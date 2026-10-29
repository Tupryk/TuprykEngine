#include <stdio.h>
#include <string.h>

#include "voxel_generators.h"
#include "../Vision/filters.h"
#include "../Algos/utils.h"


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

struct cell_t
{
    int id;
    int pos[3];
    float* channels;
    struct cell_t* neighs[3][3][3];
};

struct cell_t* new_cell(int channel_dim, int pos_x, int pos_y, int pos_z)
{
    struct cell_t* cell = (struct cell_t*) malloc(sizeof(struct cell_t));
    cell->id = -1;
    cell->pos[0] = pos_x; cell->pos[1] = pos_y; cell->pos[2] = pos_z;
    cell->channels = (float*) malloc(sizeof(float) * channel_dim);
    for (int i = 0; i < channel_dim; i++) cell->channels[i] = 0.f;
    for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
    for (int z = 0; z < 3; z++)
    cell->neighs[x][y][z] = NULL;

    cell->neighs[1][1][1] = cell;
    return cell;
}

void cell_free(void* data)
{
    struct cell_t* cell = (struct cell_t*) data;
    free(cell->channels);

    for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
    for (int z = 0; z < 3; z++)
    {
        struct cell_t* neigh = cell->neighs[x][y][z];
        if (neigh != NULL)
        {
            int neigh_x = -x + 2;
            int neigh_y = -y + 2;
            int neigh_z = -z + 2;
            neigh->neighs[neigh_x][neigh_y][neigh_z] = NULL;
        }
    }
    free(cell);
}

struct cell_t* cell_get(pstack_t* all_cells, int pos_x, int pos_y, int pos_z)
{
    struct cell_t* cell = NULL;

    if (all_cells != NULL)
    {
        struct stack_elem* current_elem = all_cells->first;
        while (current_elem != NULL)
        {
            struct cell_t* current_cell = (struct cell_t*) current_elem->data;
            if (
                current_cell->pos[0] == pos_x &&
                current_cell->pos[1] == pos_y &&
                current_cell->pos[2] == pos_z
            ) {
                cell = current_cell;
                break;
            }
            current_elem = current_elem->next;
        }
    }
    return cell;
}

struct cell_t* cell_get_or_create(
    pstack_t* all_cells, pstack_t* all_cells_next,
    int channel_dim, int pos_x, int pos_y, int pos_z,
    int* is_new
) {
    *is_new = 0;
    struct cell_t* cell = cell_get(all_cells, pos_x, pos_y, pos_z);
    if (cell == NULL) cell = cell_get(all_cells_next, pos_x, pos_y, pos_z);
    if (cell != NULL) return cell;
    *is_new = 1;
    return new_cell(channel_dim, pos_x, pos_y, pos_z);
}

void cell_populate_neighs(struct cell_t* cell, int channel_dim, pstack_t* grid, pstack_t* grid_next)
{
    int is_new;
    for (int x = 0; x < 3; x++)
    for (int y = 0; y < 3; y++)
    for (int z = 0; z < 3; z++)
    {
        if (cell->neighs[x][y][z] != NULL) continue;
        struct cell_t* neigh_cell = cell_get_or_create(
            grid,
            grid_next,
            channel_dim,
            cell->pos[0] + x - 1,
            cell->pos[1] + y - 1,
            cell->pos[2] + z - 1,
            &is_new
        );
        cell->neighs[x][y][z] = neigh_cell;

        int neigh_x = -x + 2;
        int neigh_y = -y + 2;
        int neigh_z = -z + 2;
        
        neigh_cell->neighs[neigh_x][neigh_y][neigh_z] = cell;
        if (grid_next != NULL && is_new) stack_push(grid_next, neigh_cell);
    }
}

void voxel_add_cellular(
    struct VoxelSim* vs,
    void (*cellular_automaton)(float*, float*),
    int steps, int channel_dim
) {
    // Apply convolutions to existing cells and their surroundings
    pstack_t* grid = stack_init();
    struct cell_t* seed_cell = new_cell(channel_dim, 0.f, 0.f, 0.f);
    seed_cell->channels[0] = 1.f;  // Alive
    seed_cell->channels[1] = 1.f;  // Red
    stack_push(grid, seed_cell);

    // Init seed
    cell_populate_neighs(seed_cell, channel_dim, NULL, grid);

    // Apply convolutions
    tensor_t* sobel_x = sobel_x_3d();
    tensor_t* sobel_y = sobel_y_3d();
    tensor_t* sobel_z = sobel_z_3d();

    float sum;
    float input[channel_dim * 4];
    float channel[27];
    for (int i = 0; i < steps; i++)
    {
        pstack_t* next_grid = stack_init();
        
        while (grid->first != NULL)
        {
            struct cell_t* cell = (struct cell_t*) stack_pop(grid);
            int was_alive = cell->channels[0] > 0.1f;

            for (int j = 0; j < channel_dim; j++)
            {
                for (int k = 0; k < 27; k++)
                {
                    int x = k / 9;
                    int y = (k / 3) % 3;
                    int z = k % 3;

                    struct cell_t* neigh = cell->neighs[x][y][z];
                    if (neigh != NULL) channel[k] = neigh->channels[j];
                    else  channel[k] = 0.f;
                }

                sum = 0.f;
                for (int k = 0; k < 27; k++) sum += channel[k] * sobel_x->values[k];
                input[channel_dim*1 + j] = sum;
                sum = 0.f;
                for (int k = 0; k < 27; k++) sum += channel[k] * sobel_y->values[k];
                input[channel_dim*2 + j] = sum;
                sum = 0.f;
                for (int k = 0; k < 27; k++) sum += channel[k] * sobel_z->values[k];
                input[channel_dim*3 + j] = sum;

                input[j] = cell->channels[j];
            }
            float delta[channel_dim];
            cellular_automaton(input, delta);
            float step_size = 0.05f;
            for (int c = 0; c < channel_dim; c++) cell->channels[c] += step_size * delta[c];

            if (!was_alive && cell->channels[0] > 0.1f)
            {
                cell_populate_neighs(cell, channel_dim, grid, next_grid);
            }
            
            int relevant = 0;
            for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
            for (int z = 0; z < 3; z++)
            {
                struct cell_t* neigh = cell->neighs[x][y][z];
                if (neigh != NULL && neigh->channels[0] > 0.1f) relevant = 1;
            }

            if (relevant) stack_push(next_grid, cell);
            else cell_free(cell);
        }

        stack_free(grid, cell_free);
        grid = next_grid;
    }

    tensor_free(sobel_x);
    tensor_free(sobel_y);
    tensor_free(sobel_z);

    // Transfer to voxel simulation
    int count = 0;
    float pos[grid->size *3];
    int links[grid->size * 6];
    float stiffness[grid->size];
    float cte[grid->size];
    int fixed[grid->size];
    float color[grid->size * 3];

    float min_z = 0.f;
    struct stack_elem* current_elem = grid->first;
    while (current_elem != NULL)
    {
        struct cell_t* cell = (struct cell_t*) current_elem->data;

        if (cell->channels[0] > 0.1f)
        {
            cell->id = count;

            pos[count*3 + 0] = ((float) cell->pos[0]) * vs->voxel_dim;
            pos[count*3 + 1] = ((float) cell->pos[1]) * vs->voxel_dim;
            pos[count*3 + 2] = ((float) cell->pos[2]) * vs->voxel_dim;

            if (min_z > pos[count*3 + 2]) min_z = pos[count*3 + 2];

            color[count*3 + 0] = rescale(cell->channels[1], -1.f, 1.f, 0.f, 1.f);
            color[count*3 + 1] = rescale(cell->channels[2], -1.f, 1.f, 0.f, 1.f);
            color[count*3 + 2] = rescale(cell->channels[3], -1.f, 1.f, 0.f, 1.f);

            stiffness[count] = rescale(cell->channels[4], -1.f, 1.f, 100.f, 1000.f);
            cte[count] = cell->channels[5] * 0.2f;
            fixed[count] = 0;

            count++;
        }
        
        current_elem = current_elem->next;
    }

    current_elem = grid->first;
    while (current_elem != NULL)
    {
        struct cell_t* cell = (struct cell_t*) current_elem->data;
        int id = cell->id;

        if (id != -1)
        {
            int id6 = id*6;
            // X-Axis
            links[id6 + 0] = cell->neighs[2][1][1] != NULL ? cell->neighs[2][1][1]->id : -1;
            links[id6 + 1] = cell->neighs[0][1][1] != NULL ? cell->neighs[0][1][1]->id : -1;

            // Y-Axis
            links[id6 + 2] = cell->neighs[1][2][1] != NULL ? cell->neighs[1][2][1]->id : -1;
            links[id6 + 3] = cell->neighs[1][0][1] != NULL ? cell->neighs[1][0][1]->id : -1;

            // Z-Axis
            links[id6 + 4] = cell->neighs[1][1][2] != NULL ? cell->neighs[1][1][2]->id : -1;
            links[id6 + 5] = cell->neighs[1][1][0] != NULL ? cell->neighs[1][1][0]->id : -1;
        }

        current_elem = current_elem->next;
    }

    for (int i = 0; i < count; i++) pos[i*3 + 2] += -min_z + vs->floor_z + vs->voxel_dim * 0.5f;
    stack_free(grid, cell_free);

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
