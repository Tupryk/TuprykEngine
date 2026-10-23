#include <string.h>

#include "voxel_generators.h"


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

// void voxel_add_cellular(
//     struct VoxelSim* vs,
//     void (*cellular_automaton)(float*, float*),
//     int steps, int resolution,
//     float pos_x, float pos_y, float pos_z
// ) {
//     // Apply convolutions to existing cells and their surroundings?
// }
