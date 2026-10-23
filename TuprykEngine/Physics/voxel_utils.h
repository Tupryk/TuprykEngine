#ifndef VOXEL_UTILS
#define VOXEL_UTILS

#include "voxels.h"


int voxel_is_surface(struct VoxelSim* vs, int i);
void voxel_pair_compute_forces(
    const struct VoxelSim* vs, const int i, const int j, const int axis, float* forces
);

#endif
