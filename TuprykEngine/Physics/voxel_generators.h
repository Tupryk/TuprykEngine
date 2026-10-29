#ifndef VOXEL_GENERATORS
#define VOXEL_GENERATORS

#include "voxels.h"


void voxel_add_sequence(
    struct VoxelSim* vs,
    void (*lsystem)(int, int*, float*),
    int max_steps, int max_voxels,
    float pos_x, float pos_y, float pos_z
);
void voxel_add_implicit(
    struct VoxelSim* vs,
    void (*implicit_func)(float*, float*),
    int resolution,
    float pos_x, float pos_y, float pos_z
);
void voxel_add_cellular(
    struct VoxelSim* vs,
    void (*cellular_automaton)(float*, float*),
    int steps, int channel_dim
);

#endif
