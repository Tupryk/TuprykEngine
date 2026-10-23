#ifndef VOXELS
#define VOXELS

#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"


struct VoxelSim
{
    int t;
    int count;
    int shell_count;
    int self_collision;
    int neg_x_links_count;
    int neg_y_links_count;
    int neg_z_links_count;

    // 0 -> [ 1,  0,  0]; 1 -> [-1,  0,  0];
    // 2 -> [ 0,  1,  0]; 3 -> [ 0, -1,  0];
    // 4 -> [ 0,  0,  1]; 5 -> [ 0,  0, -1];
    int* links;
    int* neg_x_links;
    int* neg_y_links;
    int* neg_z_links;
    int* fixed;
    int* shell;
    int* is_shell;
    int* neighs_count;
    int* static_flag;
    int** neighs;

    float tau;
    float voxel_dim;
    float l2;
    float l3;
    float l4;
    float I;
    float J;
    float A;
    float floor_z;
    float temp;
    float temp_ref;
    float damping_ratio;
    float mu_d;
    float mu_s;
    float gravity;
    float max_motion;
    
    tensor_t* pos;
    tensor_t* vel;
    tensor_t* acc;

    tensor_t* rot;
    tensor_t* rot_vel;
    tensor_t* rot_acc;

    float* mass;
    float* inertia;
    float* stiffness;
    float* shear;
    float* cte;
    float* a1;
    float* a2;
    float* b1;
    float* b2;
    float* b3;
    float* damping_scaler;
    float* damping_scaler_rot;

    tensor_t* color;

    pstack_t* coll_pairs;
};

struct VoxelSim* voxel_init(float voxel_dim, int self_collision);
void voxel_expand(
    struct VoxelSim* vs,
    int count,
    float* pos,
    int* links,
    float* stiffness,
    float* cte,
    int* fixed,
    float* color
);
void voxel_free(struct VoxelSim* vs);
void voxel_step(struct VoxelSim* vs);

#endif
