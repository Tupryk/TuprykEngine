// Based on https://arxiv.org/pdf/1212.2845

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "voxels.h"
#include "voxel_utils.h"
#include "../Funcs/funcs.h"
#include "../Algos/lists.h"
#include "../Geom/quaternions.h"


void voxel_compute_outer_shell(struct VoxelSim* vs)
{
    vs->shell_count = 0;
    for (int i = 0; i < vs->count; i++)
    {
        if (voxel_is_surface(vs, i))
        {
            vs->shell_count++;
        }
    }
    
    if (vs->shell != NULL) free(vs->shell);
    if (vs->is_shell != NULL) free(vs->is_shell);
    vs->shell = (int*) malloc(sizeof(int) * vs->shell_count);
    vs->is_shell = (int*) malloc(sizeof(int) * vs->count);

    int counter = 0;
    for (int i = 0; i < vs->count; i++)
    {
        vs->is_shell[i] = 0;
        if (voxel_is_surface(vs, i))
        {
            vs->is_shell[i] = 1;
            vs->shell[counter] = i;
            counter++;
        }
    }
}

void voxel_get_neighs(struct VoxelSim* vs, int voxel_id, int* neighs, int* neigh_count, int depth, int max_depth)
{
    if (depth >= max_depth) return;

    for (int i = 0; i < 6; i++)
    {
        int linked = vs->links[voxel_id*6 + i];
        if (linked != -1 && vs->is_shell[voxel_id])
        {
            neighs[*neigh_count] = linked;
            *neigh_count += 1;
            voxel_get_neighs(vs, linked, neighs, neigh_count, depth+1, max_depth);
        }
    }
}

void voxel_compute_neighs(struct VoxelSim* vs)
{   
    vs->neighs = (int**) malloc(sizeof(int*) * vs->shell_count);
    vs->neighs_count = (int*) malloc(sizeof(int) * vs->shell_count);
    
    int max_depth = 3;
    int max_neighs = 1;
    for (int i = 0; i < max_depth; i++) max_neighs *= 6;
    int neighs[max_neighs];

    for (int i = 0; i < vs->shell_count; i++)
    {
        int voxel_id = vs->shell[i];
        int neigh_count = 0;
        voxel_get_neighs(vs, voxel_id, neighs, &neigh_count, 0, max_depth);
        vs->neighs_count[i] = neigh_count;

        if (neigh_count)
        {
            vs->neighs[i] = (int*) malloc(sizeof(int) * neigh_count);
            for (int j = 0; j < neigh_count; j++)
            {
                vs->neighs[i][j] = neighs[j];
            }
        }
        else
        {
            vs->neighs[i] = NULL;
        }
    }
}

void voxel_compute_collision_horizon(struct VoxelSim* vs)
{
    if (vs->coll_pairs != NULL) stack_free(vs->coll_pairs, free);
    vs->coll_pairs = stack_init();

    float horizon = vs->voxel_dim * 2.f;
    for (int i = 0; i < vs->shell_count; i++)
    {
        for (int j = i+1; j < vs->shell_count; j++)
        {
            int id1 = vs->shell[i];
            int id2 = vs->shell[j];

            // Within range
            float* p1 = &vs->pos->values[id1*3];
            float* p2 = &vs->pos->values[id2*3];
            float dir[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
            float dist = sqrtf(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);

            if (dist > horizon) continue;
            
            // Are neighbours
            int are_neighs = 0;
            for (int k = 0; k < vs->neighs_count[i]; k++)
            {
                if (vs->neighs[i][k] == id2)
                {
                    are_neighs = 1;
                    break;
                }
            }
            if (are_neighs) continue;

            // Add to collision pairs
            int* coll_pair = (int*) malloc(sizeof(int) * 2);
            coll_pair[0] = id1; coll_pair[1] = id2;
            stack_push(vs->coll_pairs, coll_pair);
        }
    }
}

struct VoxelSim* voxel_init(float voxel_dim, int self_collision)
{
    struct VoxelSim* vs = (struct VoxelSim*) malloc(sizeof(struct VoxelSim));

    vs->t = 0;
    vs->count = 0;
    vs->shell_count = 0;
    vs->neg_x_links_count = 0;
    vs->neg_y_links_count = 0;
    vs->neg_z_links_count = 0;
    vs->links = NULL;
    vs->neg_x_links = NULL;
    vs->neg_y_links = NULL;
    vs->neg_z_links = NULL;
    
    vs->voxel_dim = voxel_dim;
    vs->tau = 0.0001f;
    vs->floor_z = -1.1f;
    vs->temp = 0.1f;
    vs->temp_ref = 0.f;
    vs->damping_ratio = 1.4f;
    vs->mu_d = 0.8f;
    vs->mu_s = 1.0f;
    vs->gravity = -9.81f;
    vs->max_motion = voxel_dim * 2.1f;
    vs->self_collision = self_collision;

    // Helpers
    vs->l2 = voxel_dim * voxel_dim;
    vs->l3 = vs->l2 * voxel_dim;
    vs->l4 = vs->l3 * voxel_dim;
    
    vs->I = vs->l4 / 12.f;
    vs->J = vs->l4 / 6.f;
    vs->A = vs->l2;

    vs->pos = NULL;
    vs->vel = NULL;
    vs->acc = NULL;

    vs->rot = NULL;
    vs->rot_vel = NULL;
    vs->rot_acc = NULL;

    vs->mass = NULL;
    vs->inertia = NULL;
    vs->stiffness = NULL;
    vs->shear = NULL;
    vs->cte = NULL;
    vs->damping_scaler = NULL;
    vs->damping_scaler_rot = NULL;

    vs->coll_pairs = NULL;
    
    vs->color = NULL;
    
    vs->shell = NULL;
    vs->is_shell = NULL;
    vs->neighs = NULL;
    vs->neighs_count = NULL;

    vs->static_flag = NULL;

    return vs;
}

void voxel_expand(
    struct VoxelSim* vs,
    int count,
    float* pos,
    int* links,
    float* stiffness,
    float* cte,
    int* fixed,
    float* color
) {
    int prev_count = vs->count;
    int new_count = prev_count + count;
    vs->count = new_count;
    
    if (vs->vel != NULL)
    {
        tensor_free(vs->vel);
        tensor_free(vs->acc);

        tensor_free(vs->rot);
        tensor_free(vs->rot_vel);
        tensor_free(vs->rot_acc);

        free(vs->mass);
        free(vs->static_flag);
    }

    // Position
    if (vs->pos == NULL)
    {
        vs->pos = new_tensor_matrix(count, 3, NULL);
    }
    else
    {
        vs->pos->shape[0] = new_count;
        vs->pos->volume = new_count * 3;
        vs->pos->values = realloc(vs->pos->values, sizeof(float) * new_count * 3);
    }
    vs->vel = new_tensor_matrix(new_count, 3, NULL);
    vs->acc = new_tensor_matrix(new_count, 3, NULL);

    // Rotation
    vs->rot     = new_tensor_matrix(new_count, 4, NULL);
    vs->rot_vel = new_tensor_matrix(new_count, 3, NULL);
    vs->rot_acc = new_tensor_matrix(new_count, 3, NULL);
    for (int i = 0; i < new_count; i++) vs->rot->values[i*4] = 1.f;

    // Material properties
    if (vs->stiffness == NULL)
    {
        vs->inertia = (float*) malloc(sizeof(float) * count);
        vs->stiffness = (float*) malloc(sizeof(float) * count);
        vs->shear = (float*) malloc(sizeof(float) * count);
        vs->cte = (float*) malloc(sizeof(float) * count);

        vs->a1 = (float*) malloc(sizeof(float) * count*3);
        vs->a2 = (float*) malloc(sizeof(float) * count*3);
        vs->b1 = (float*) malloc(sizeof(float) * count*3);
        vs->b2 = (float*) malloc(sizeof(float) * count*3);
        vs->b3 = (float*) malloc(sizeof(float) * count*3);

        vs->damping_scaler = (float*) malloc(sizeof(float) * count*6);
        vs->damping_scaler_rot = (float*) malloc(sizeof(float) * count*6);
    }
    else
    {
        vs->inertia = realloc(vs->inertia, sizeof(float) * new_count);
        vs->stiffness = realloc(vs->stiffness, sizeof(float) * new_count);
        vs->shear = realloc(vs->shear, sizeof(float) * new_count);
        vs->cte = realloc(vs->cte, sizeof(float) * new_count);

        vs->a1 = realloc(vs->a1, sizeof(float) * new_count*3);
        vs->a2 = realloc(vs->a2, sizeof(float) * new_count*3);
        vs->b1 = realloc(vs->b1, sizeof(float) * new_count*3);
        vs->b2 = realloc(vs->b2, sizeof(float) * new_count*3);
        vs->b3 = realloc(vs->b3, sizeof(float) * new_count*3);

        vs->damping_scaler = realloc(vs->damping_scaler, sizeof(float) * new_count*6);
        vs->damping_scaler_rot = realloc(vs->damping_scaler_rot, sizeof(float) * new_count*6);
    }

    vs->mass = (float*) malloc(sizeof(float) * new_count);
    for (int i = 0; i < new_count; i++) vs->mass[i] = vs->l3;
    
    // Color
    if (vs->color == NULL)
    {
        vs->color = new_tensor_matrix(count, 3, NULL);
    }
    else
    {
        vs->color->shape[0] = new_count;
        vs->color->volume = new_count * 3;
        vs->color->values = realloc(vs->color->values, sizeof(float) * new_count * 3);
    }

    if (vs->links == NULL)
    {
        // Links
        vs->links = (int*) malloc(sizeof(int) * count * 6);
        vs->neg_x_links = (int*) malloc(sizeof(int) * count);
        vs->neg_y_links = (int*) malloc(sizeof(int) * count);
        vs->neg_z_links = (int*) malloc(sizeof(int) * count);
    
        // Fixed voxels
        vs->fixed = (int*) malloc(sizeof(int) * count);
    }
    else
    {
        vs->links = realloc(vs->links, sizeof(int) * new_count * 6);
        vs->neg_x_links = realloc(vs->neg_x_links, sizeof(int) * new_count);
        vs->neg_y_links = realloc(vs->neg_y_links, sizeof(int) * new_count);
        vs->neg_z_links = realloc(vs->neg_z_links, sizeof(int) * new_count);
        vs->fixed = realloc(vs->fixed, sizeof(int) * new_count);
    }

    float shear_tmp = (2.f*(1.f+0.33f));

    for (int i = prev_count; i < new_count; i++)
    {
        vs->inertia[i] = vs->mass[i] * vs->l2 / 6.f;
        vs->stiffness[i] = stiffness[i-prev_count];
        vs->shear[i] = vs->stiffness[i] / shear_tmp;
        vs->cte[i] = cte[i-prev_count];
    }

    for (int i = prev_count; i < new_count; i++)
    {
        int i3 = i*3;
        int i6 = i*6;

        for (int j = 0; j < 6; j++)
        {
            int neigh_id = links[i6 - prev_count*6 + j];
            vs->links[i6 + j] = neigh_id;
            if (vs->links[i6 + j] != -1)
            {
                if (j == 1)
                {
                    vs->neg_x_links[vs->neg_x_links_count] = i;
                    vs->neg_x_links_count++;
                }
                if (j == 3)
                {
                    vs->neg_y_links[vs->neg_y_links_count] = i;
                    vs->neg_y_links_count++;
                }
                if (j == 5)
                {
                    vs->neg_z_links[vs->neg_z_links_count] = i;
                    vs->neg_z_links_count++;
                }
                vs->links[i6 + j] += prev_count;
                neigh_id += prev_count;
                if (j % 2 == 1)
                {
                    float E1 = vs->stiffness[i];
                    float E2 = vs->stiffness[neigh_id];

                    float G1 = vs->shear[i];
                    float G2 = vs->shear[neigh_id];

                    float Ec = (2.f * E1 * E2) / (E1 + E2);
                    float Gc = (2.f * G1 * G2) / (G1 + G2);

                    int j2 = j/2;
                    int j1 = j-1;
                    vs->a1[i3+j2] = (Ec * vs->A) / vs->voxel_dim;
                    vs->a2[i3+j2] = (Gc * vs->J) / vs->voxel_dim;

                    vs->b1[i3+j2] = (12.f * Ec * vs->I) / vs->l3;
                    vs->b2[i3+j2] = (6.f * Ec * vs->I) / vs->l2;
                    vs->b3[i3+j2] = (2.f * Ec * vs->I) / vs->voxel_dim;

                    float m1 = vs->mass[i];
                    float m2 = vs->mass[neigh_id];
                    float k_ax  = vs->a1[i3+j2];
                    float k_rot = 2.f * vs->b3[i3+j2];
                    float I_rot1 = m1 * vs->l2 / 6.f;
                    float I_rot2 = m2 * vs->l2 / 6.f;

                    int i6 = i*6;
                    vs->damping_scaler[i6+j2] = 2.f * vs->damping_ratio * sqrtf(m1 * k_ax);
                    vs->damping_scaler_rot[i6+j2] = 2.f * vs->damping_ratio * sqrtf(I_rot1 * k_rot);

                    int j6 = neigh_id*6;
                    vs->damping_scaler[j6+j1] = 2.f * vs->damping_ratio * sqrtf(m2 * k_ax);
                    vs->damping_scaler_rot[j6+j1] = 2.f * vs->damping_ratio * sqrtf(I_rot2 * k_rot);
                }
            }
        }

        vs->pos->values[i3] = pos[i3-prev_count*3];
        vs->pos->values[i3+1] = pos[i3-prev_count*3+1];
        vs->pos->values[i3+2] = pos[i3-prev_count*3+2];
        
        vs->color->values[i3] = color[i3-prev_count*3];
        vs->color->values[i3+1] = color[i3-prev_count*3+1];
        vs->color->values[i3+2] = color[i3-prev_count*3+2];

        vs->fixed[i] = fixed[i-prev_count];
    }

    if (vs->neighs_count != NULL)
    {
        free(vs->neighs_count);
        for (int i = 0; i < vs->shell_count; i++)
        {
            if (vs->neighs[i] != NULL) free(vs->neighs[i]);
        }
        free(vs->neighs);
        vs->neighs = NULL;
        vs->neighs_count = NULL;
    }

    // Pre-compute outer shell for faster collision checks
    voxel_compute_outer_shell(vs);
    if (vs->self_collision) voxel_compute_neighs(vs);

    vs->static_flag = (int*) malloc(sizeof(int) * vs->shell_count);
    memset(vs->static_flag, 0, sizeof(int) * vs->shell_count);
}

void move_voxel(int id, int axis, int* pos, int* links)
{
    int_stack_t* pushing = int_stack_init();
    int_stack_t* pushed = int_stack_init();
    int_stack_push(pushing, id);
    int_stack_push(pushed, id);

    int ax = axis/2;
    int dir = axis%2 ? -1 : 1;
    
    while (pushing->size)
    {
        int pushee = int_stack_pop(pushing);
        pos[pushee*3 + ax] += dir;
        
        for (int i = 0; i < 6; i++)
        {
            int neigh_id = links[pushee*6 + i];
            if (neigh_id != -1 && !int_stack_contains(pushed, neigh_id))
            {
                int_stack_push(pushing, neigh_id);
                int_stack_push(pushed, neigh_id);
            }
        }
    }

    int_stack_free(pushing);
    int_stack_free(pushed);
}

void voxel_add_sequence(
    struct VoxelSim* vs,
    void (*lsystem)(int, int*, float*),
    int max_steps, int max_voxels,
    float pos_x, float pos_y, float pos_z
) {
    float params[6];
    int out[7];

    int types[max_voxels];
    
    int pos[max_voxels * 3];
    int links[max_voxels * 6];
    float stiffness[max_voxels];
    float cte[max_voxels];
    float color[max_voxels * 3];
    int fixed[max_voxels];

    memset(links, -1, sizeof(links));

    types[0] = 0;
    int count = 1;
    pos[0] = 0; pos[1] = 0; pos[2] = 0;
    
    for (int i = 0; i < max_steps; i++)
    {
        int last_size = count;
        for (int parent = 0; parent < last_size; parent++)
        {
            lsystem(types[parent], out, params);

            for (int k = 0; k < 6; k++)
            {
                int child = count;
                if (child >= max_voxels) break;

                if (out[k] != -1)
                {
                    types[child] = out[k];
                    
                    int back_dir = k%2 ? k - 1 : k + 1;

                    int old_neigh = links[parent*6 + k];
                    if (old_neigh != -1)
                    {
                        links[old_neigh*6 + back_dir] = -1;
                        move_voxel(old_neigh, k, pos, links);
                        links[old_neigh*6 + back_dir] = child;
                        links[child*6 + k] = old_neigh;
                    }
                    
                    links[parent*6 + k] = child;
                    links[child*6 + back_dir] = parent;

                    pos[child*3 + 0] = pos[parent*3 + 0];
                    pos[child*3 + 1] = pos[parent*3 + 1];
                    pos[child*3 + 2] = pos[parent*3 + 2];

                    pos[child*3 + k/2] += k%2 ? -1 : 1;

                    count++;
                }
            }
            if (out[6] != -1) types[parent] = out[6];
        }
    }
    
    float posf[max_voxels*3];
    for (int i = 0; i < count; i++)
    {
        int i3 = i*3;
        posf[i3  ] = ((float) pos[i3  ]) * vs->voxel_dim + pos_x;
        posf[i3+1] = ((float) pos[i3+1]) * vs->voxel_dim + pos_y;
        posf[i3+2] = ((float) pos[i3+2]) * vs->voxel_dim + pos_z;

        lsystem(types[i], out, params);

        stiffness[i] = params[0];
        cte[i] = params[1];
        
        color[i3 + 0] = params[2];
        color[i3 + 1] = params[3];
        color[i3 + 2] = params[4];
        
        fixed[i] = params[5] > 0.f;
    }
    voxel_expand(
        vs,
        count,
        posf,
        links,
        stiffness,
        cte,
        fixed,
        color
    );
}

void voxel_add_implicit(
    struct VoxelSim* vs,
    void (*implicit_func)(float*, float*),
    int resolution,
    float pos_x, float pos_y, float pos_z
) {
    int occupied_values[resolution][resolution][resolution];
    memset(occupied_values, -1, sizeof(occupied_values));

    int res3 = resolution * resolution * resolution;
    
    float input[3];
    float output[7];
    
    float pos[res3 * 3];
    int links[res3 * 6];
    float stiffness[res3];
    float cte[res3];
    float color[res3 * 3];
    int fixed[res3];
    
    memset(links, -1, sizeof(links));
    float cube_size = ((float) resolution) * vs->voxel_dim;

    int count = 0;
    for (int x = 0; x < resolution; x++)
    {
        for (int y = 0; y < resolution; y++)
        {
            for (int z = 0; z < resolution; z++)
            {
                input[0] = ((float) x) / ((float) resolution) * cube_size - cube_size * 0.5f;
                input[1] = ((float) y) / ((float) resolution) * cube_size - cube_size * 0.5f;
                input[2] = ((float) z) / ((float) resolution) * cube_size - cube_size * 0.5f;
    
                implicit_func(input, output);

                if (output[0] > 0.1f)
                {
                    int count3 = count * 3;

                    pos[count3 + 0] = input[0] + pos_x;
                    pos[count3 + 1] = input[1] + pos_y;
                    pos[count3 + 2] = input[2] + pos_z;

                    occupied_values[x][y][z] = count;

                    // Fill links
                    int i6 = count*6;
                    int neigh_voxel = -1;
                    
                    if (x != 0) neigh_voxel = occupied_values[x-1][y][z];
                    else neigh_voxel = -1;
                    if (neigh_voxel != -1) { links[i6 + 1] = neigh_voxel; links[neigh_voxel*6 + 0] = count; }
                    
                    if (y != 0) neigh_voxel = occupied_values[x][y-1][z];
                    else neigh_voxel = -1;
                    if (neigh_voxel != -1) { links[i6 + 3] = neigh_voxel; links[neigh_voxel*6 + 2] = count; }
                    
                    if (z != 0) neigh_voxel = occupied_values[x][y][z-1];
                    else neigh_voxel = -1;
                    if (neigh_voxel != -1) { links[i6 + 5] = neigh_voxel; links[neigh_voxel*6 + 4] = count; }

                    // Fill physical params
                    stiffness[count] = output[1];
                    cte[count] = output[2];
                    
                    // Fill color
                    color[count3 + 0] = output[3];
                    color[count3 + 1] = output[4];
                    color[count3 + 2] = output[5];
                    
                    // Fill fixed
                    fixed[count] = output[6] > 0.f;

                    // Increase count
                    count++;
                }
            }
        }
    }

    if (count)
    {
        voxel_expand(
            vs,
            count,
            pos,
            links,
            stiffness,
            cte,
            fixed,
            color
        );
    }
}

void voxel_free(struct VoxelSim* vs)
{
    if (vs->pos != NULL)
    {
        tensor_free(vs->pos);
        tensor_free(vs->vel);
        tensor_free(vs->acc);
    
        tensor_free(vs->rot);
        tensor_free(vs->rot_vel);
        tensor_free(vs->rot_acc);
    
        free(vs->mass);
        free(vs->inertia);
        free(vs->stiffness);
        free(vs->shear);
        free(vs->cte);
    
        free(vs->a1);
        free(vs->a2);
        free(vs->b1);
        free(vs->b2);
        free(vs->b3);
    
        free(vs->damping_scaler);
        free(vs->damping_scaler_rot);
    
        tensor_free(vs->color);
    
        free(vs->links);
        free(vs->neg_x_links);
        free(vs->neg_y_links);
        free(vs->neg_z_links);
        free(vs->fixed);
        free(vs->static_flag);
        
        free(vs->shell);
        free(vs->is_shell);
        if (vs->neighs_count != NULL)
        {
            free(vs->neighs_count);
            for (int i = 0; i < vs->shell_count; i++) if (vs->neighs[i] != NULL) free(vs->neighs[i]);
            free(vs->neighs);
        }

        if (vs->coll_pairs != NULL) stack_free(vs->coll_pairs, free);
    }
    free(vs);
}

void voxel_compute_acc(struct VoxelSim* vs)
{
    float forces[12];

    tensor_fill(vs->acc, 0.f);
    tensor_fill(vs->rot_acc, 0.f);

    // Negative X links
    for (int x = 0; x < vs->neg_x_links_count; x++)
    {
        int i = vs->neg_x_links[x];
        int i6 = i*6;
        int neigh_id = vs->links[i6 + 1];
        voxel_pair_compute_forces(vs, i, neigh_id, 1, forces);
    }
    // Negative Y links
    for (int x = 0; x < vs->neg_y_links_count; x++)
    {
        int i = vs->neg_y_links[x];
        int i6 = i*6;
        int neigh_id = vs->links[i6 + 3];
        voxel_pair_compute_forces(vs, i, neigh_id, 3, forces);
    }
    // Negative Z links
    for (int x = 0; x < vs->neg_z_links_count; x++)
    {
        int i = vs->neg_z_links[x];
        int i6 = i*6;
        int neigh_id = vs->links[i6 + 5];
        voxel_pair_compute_forces(vs, i, neigh_id, 5, forces);
    }

    for (int i = 0; i < vs->count; i++)
    {
        int i3 = i * 3;
        
        float inv_mass = 1.0f / vs->mass[i];
        float inv_inertia = 1.0f / vs->inertia[i];

        vs->acc->values[i3]     *= inv_mass;
        vs->acc->values[i3 + 1] *= inv_mass;
        vs->acc->values[i3 + 2]  = vs->acc->values[i3 + 2] * inv_mass
                                   + vs->gravity;

        vs->rot_acc->values[i3]     *= inv_inertia;
        vs->rot_acc->values[i3 + 1] *= inv_inertia;
        vs->rot_acc->values[i3 + 2] *= inv_inertia;
    }
}

void voxel_compute_collisions(struct VoxelSim* vs)
{
    float* pos = vs->pos->values;
    float* vel = vs->vel->values;
    float* acc = vs->acc->values;

    for (int j = 0; j < vs->shell_count; j++)
    {
        int i = vs->shell[j];
        int i3 = i*3;
        
        float m1 = vs->mass[i];
        
        float Fx = acc[i3] * m1;
        float Fy = acc[i3+1] * m1;
        
        float k_floor = vs->stiffness[i] * vs->voxel_dim;
        float delta = vs->floor_z - (pos[i3 + 2] - 0.5f * vs->voxel_dim);
        
        float vx = vel[i3 + 0];
        float vy = vel[i3 + 1];
        float vz = vel[i3 + 2];
    
        // Table
        if (delta > 0.f)
        {
            float c  = 2.f * vs->damping_ratio * sqrtf(m1 * k_floor);
            float Fn = k_floor * delta - (vz < 0.f ? c * vz : 0.f);
            if (Fn < 0.f) Fn = 0.f;
            acc[i3+2] += Fn / m1;

            float Fl = sqrtf(Fx*Fx + Fy*Fy);
            float vl = sqrtf(vx*vx + vy*vy);
            int zero_vel = 0;

            if (vs->static_flag[j]) {
                if (Fl <= vs->mu_s * Fn) {
                    Fx = Fy = 0.f; zero_vel = 1;
                } else {
                    vs->static_flag[j] = 0;
                    float Ff = vs->mu_d * Fn;
                    if (Fl > 1e-9f) { Fx -= Ff*Fx/Fl; Fy -= Ff*Fy/Fl; }
                }
            } else {
                if (vl <= Fn * vs->mu_d * vs->tau / m1) {
                    Fx = Fy = 0.f; zero_vel = 1;
                    vs->static_flag[j] = 1;
                } else {
                    Fx -= vs->mu_d * Fn * vx / vl;
                    Fy -= vs->mu_d * Fn * vy / vl;
                }
            }

            acc[i3]   = Fx / m1;
            acc[i3+1] = Fy / m1;
            if (zero_vel) { vs->vel->values[i3] = 0.f; vs->vel->values[i3+1] = 0.f; }
        }
        else
        {
            vs->static_flag[j] = 0;
        }
    }

    if (vs->self_collision || vs->coll_pairs != NULL)
    {
        struct stack_elem* current_pair = vs->coll_pairs->first;
        while (current_pair != NULL)
        {
            int* pair = (int*) current_pair->data;
            int i = pair[0]; int j = pair[1];
            int i3 = i*3; int j3 = j*3;
            
            float m1 = vs->mass[i];
            float m2 = vs->mass[j];

            float k1 = vs->stiffness[i];
            float k2 = vs->stiffness[j];

            float* p1 = &vs->pos->values[i3];
            float* p2 = &vs->pos->values[j3];
            float dir[3] = { p2[0] - p1[0], p2[1] - p1[1], p2[2] - p1[2] };
            float dist = sqrtf(dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2]);
            float delta = vs->voxel_dim - dist;

            if (dist > 1e-6f && delta > 0.f)
            {
                float n[3] = { dir[0]/dist, dir[1]/dist, dir[2]/dist };

                float vr[3] = { vel[j3]-vel[i3], vel[j3+1]-vel[i3+1], vel[j3+2]-vel[i3+2] };
                float vn = vr[0]*n[0] + vr[1]*n[1] + vr[2]*n[2];
                float vt[3] = { vr[0]-vn*n[0], vr[1]-vn*n[1], vr[2]-vn*n[2] };
                float vt_mag = sqrtf(vt[0]*vt[0] + vt[1]*vt[1] + vt[2]*vt[2]);

                float m_eff = m1*m2 / (m1 + m2);
                float k = (k1*k2/(k1+k2)) * vs->voxel_dim;
                float c = 2.f * vs->damping_ratio * sqrtf(m_eff * k);

                float Fn = k*delta - (vn < 0.f ? c*vn : 0.f);
                if (Fn < 0.f) Fn = 0.f;

                float F[3] = { Fn*n[0], Fn*n[1], Fn*n[2] };

                if (vt_mag > 1e-6f)
                {
                    float Ft = vs->mu_d * Fn;
                    float Ft_stop = m_eff * vt_mag / vs->tau;
                    if (Ft > Ft_stop) Ft = Ft_stop;
                    for (int a = 0; a < 3; a++) F[a] -= Ft * vt[a] / vt_mag;
                }

                for (int a = 0; a < 3; a++) {
                    acc[j3+a] += F[a] / m2;
                    acc[i3+a] -= F[a] / m1;
                }
            }
            current_pair = current_pair->next;
        }
    }
}

void voxel_integrate(struct VoxelSim* vs)
{
    // Position and velocity updates
    tensor_add_delta(vs->vel, vs->acc, vs->tau, vs->vel);
    tensor_add_delta(vs->pos, vs->vel, vs->tau, vs->pos);

    tensor_add_delta(vs->rot_vel, vs->rot_acc, vs->tau, vs->rot_vel);

    float max_vel2 = 0.f;
    for (int i = 0; i < vs->count; i++)
    {
        float* vel = &vs->vel->values[i*3];
        float vel_mag2 = vel[0]*vel[0] + vel[1]*vel[1] + vel[2]*vel[2];
        if (max_vel2 < vel_mag2) max_vel2 = vel_mag2;

        quaternion_delta(&vs->rot->values[i*4], &vs->rot_vel->values[i*3], vs->tau, &vs->rot->values[i*4]);
    }

    vs->max_motion += sqrtf(max_vel2) * vs->tau;
}

void voxel_step(struct VoxelSim* vs)
{
    clock_t total_time = clock();
    clock_t acc_time = clock();
    voxel_compute_acc(vs);
    acc_time = clock() - acc_time;

    clock_t coll_reload_time = clock();
    if (vs->self_collision && vs->max_motion >= vs->voxel_dim * 0.5f)
    {
        voxel_compute_collision_horizon(vs);
        vs->max_motion = 0.f;
    }
    coll_reload_time = clock() - coll_reload_time;
    
    clock_t coll_time = clock();
    voxel_compute_collisions(vs);
    coll_time = clock() - coll_time;

    for (int i = 0; i < vs->count; i++)
    {
        if (vs->fixed[i])
        {
            int i3 = i*3;

            vs->acc->values[i3    ] = 0.f;
            vs->acc->values[i3 + 1] = 0.f;
            vs->acc->values[i3 + 2] = 0.f;
    
            vs->rot_acc->values[i3    ] = 0.f;
            vs->rot_acc->values[i3 + 1] = 0.f;
            vs->rot_acc->values[i3 + 2] = 0.f;
        }
    }
    
    clock_t integr_time = clock();
    voxel_integrate(vs);
    integr_time = clock() - integr_time;
    
    vs->t++;
    
    // total_time = clock() - total_time;
    // printf("+----------------------------------+\n");
    // printf(
    //     "Total time: %.3f ms (%.3f Acc, %.3f Coll Reload, %.3f Coll, %.3f Integr)\n",
    //     (((float)total_time) / CLOCKS_PER_SEC) * 1000.f,
    //     ((float) acc_time) / ((float)total_time) * 100.f,
    //     ((float) coll_reload_time) / ((float)total_time) * 100.f,
    //     ((float) coll_time) / ((float)total_time) * 100.f,
    //     ((float) integr_time) / ((float)total_time) * 100.f
    // );
}
