#ifndef HANDS_SCRIPT_UTILS
#define HANDS_SCRIPT_UTILS

#include "../../TuprykEngine/Physics/voxels.h"

void human_made_sequence_hand0(int type, int* out, float* params);
void human_made_sequence_hand1(int type, int* out, float* params);
void human_made_implicit_hand(float* pos, float* out);
void human_made_implicit_cube(float* pos, float* out);
void fixed_grasp_motion(struct VoxelSim* vs, float start_time, float end_time, float z_depth);

#endif
