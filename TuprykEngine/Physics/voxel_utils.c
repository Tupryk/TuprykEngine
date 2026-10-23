#include <stdio.h>
#include <math.h>

#include "voxel_utils.h"
#include "../Geom/quaternions.h"


int voxel_is_surface(struct VoxelSim* vs, int i)
{
    int* links = &vs->links[i*6];
    return (links[0] == -1 || links[1] == -1 || links[2] == -1 || links[3] == -1 || links[4] == -1 || links[5] == -1);
}

void rotate_to_posX(int axis, float* x, float* y, float* z)
{
    float vx = *x, vy = *y, vz = *z;
    switch (axis) {
    case 0: break;                      // +X: identity
    case 1: *x = -vx; *y = -vy; break;  // -X: 180 deg about Z
    case 2: *x =  vy; *y = -vx; break;  // +Y: -90 deg about Z
    case 3: *x = -vy; *y =  vx; break;  // -Y: +90 deg about Z
    case 4: *x =  vz; *z = -vx; break;  // +Z: +90 deg about Y
    case 5: *x = -vz; *z =  vx; break;  // -Z: -90 deg about Y
    }
}

void rotate_from_posX(int axis, float* x, float* y, float* z)
{
    float vx = *x, vy = *y, vz = *z;
    switch (axis) {
    case 0: break;
    case 1: *x = -vx; *y = -vy; break;
    case 2: *x = -vy; *y =  vx; break;
    case 3: *x =  vy; *y = -vx; break;
    case 4: *x = -vz; *z =  vx; break;
    case 5: *x =  vz; *z = -vx; break;
    }
}

void compute_damping(
    const struct VoxelSim* vs,
    int i, int j,
    float* rel_vel,
    float* rel_rot_vel)
{
    int i3 = i * 3;
    int j3 = j * 3;

    const float* pos = vs->pos->values;
    const float* vel = vs->vel->values;
    const float* rot_vel = vs->rot_vel->values;

    float dx = 0.5f * (pos[j3]     - pos[i3]);
    float dy = 0.5f * (pos[j3 + 1] - pos[i3 + 1]);
    float dz = 0.5f * (pos[j3 + 2] - pos[i3 + 2]);

    float wx = 0.5f * (rot_vel[i3]     + rot_vel[j3]);
    float wy = 0.5f * (rot_vel[i3 + 1] + rot_vel[j3 + 1]);
    float wz = 0.5f * (rot_vel[i3 + 2] + rot_vel[j3 + 2]);

    rel_vel[0] = 0.5f * (vel[j3]     - vel[i3])     + dy * wz - dz * wy;
    rel_vel[1] = 0.5f * (vel[j3 + 1] - vel[i3 + 1]) + dz * wx - dx * wz;
    rel_vel[2] = 0.5f * (vel[j3 + 2] - vel[i3 + 2]) + dx * wy - dy * wx;

    rel_rot_vel[0] = 0.5f * (rot_vel[j3]     - rot_vel[i3]);
    rel_rot_vel[1] = 0.5f * (rot_vel[j3 + 1] - rot_vel[i3 + 1]);
    rel_rot_vel[2] = 0.5f * (rot_vel[j3 + 2] - rot_vel[i3 + 2]);
}

void voxel_pair_compute_forces(
    const struct VoxelSim* vs, const int i, const int j, const int axis, float* forces
) {
    // Calculates link forces
    int i3 = i*3;
    int j3 = j*3;
    int i6 = i*6;
    int j6 = j*6;
    int j2 = axis/2;
    int j1 = axis - 1;

    float* pos = vs->pos->values;
    float* rot = vs->rot->values;
    float* acc = vs->acc->values;
    float* rot_acc = vs->rot_acc->values;

    float l = vs->voxel_dim;

    float X2 = pos[j3  ] - pos[i3  ];
    float Y2 = pos[j3+1] - pos[i3+1];
    float Z2 = pos[j3+2] - pos[i3+2];
    
    float qi_conj[4];
    quaternion_conjugate(&rot[i*4], qi_conj);

    float d_global[3] = { X2, Y2, Z2 }, d_local[3];
    quaternion_rotate(qi_conj, d_global, d_local);
    X2 = d_local[0]; Y2 = d_local[1]; Z2 = d_local[2];

    float q_rel[4];
    float theta[3];
    quaternion_diff(&rot[i*4], &rot[j*4], q_rel);
    quaternion_to_rotvec(q_rel, theta);

    float thetaX2 = theta[0];
    float thetaY2 = theta[1];
    float thetaZ2 = theta[2];

    float alpha1 = vs->cte[i];
    float alpha2 = vs->cte[j];
    float l_rest = l * (1.f + 0.5f * (alpha1 + alpha2) * (vs->temp - vs->temp_ref));

    rotate_to_posX(axis, &X2, &Y2, &Z2);
    rotate_to_posX(axis, &thetaX2, &thetaY2, &thetaZ2);
    X2 -= l_rest;

    int abidx = i3+j2;
    float a1 = vs->a1[abidx]; float a2 = vs->a2[abidx];
    float b1 = vs->b1[abidx]; float b2 = vs->b2[abidx]; float b3 = vs->b3[abidx];

    // Damping
    float V21[3], omega21[3];

    compute_damping(vs, i, j, V21, omega21);

    float V12[3] = {-V21[0], -V21[1], -V21[2]};
    float omega12[3] = {
        -omega21[0], -omega21[1], -omega21[2]
    };

    float damping_scaler1 = vs->damping_scaler[i6+j2];
    float damping_scaler_rot1 = vs->damping_scaler_rot[i6+j2];

    float damping_scaler2 = vs->damping_scaler[j6+j1];
    float damping_scaler_rot2 = vs->damping_scaler_rot[j6+j1];

    forces[0] = -a1*X2;
    forces[1] = -b1*Y2 + b2*thetaZ2;
    forces[2] = -b1*Z2 - b2*thetaY2;

    forces[3] = -a2*thetaX2;
    forces[4] = b2*Z2 + b3*thetaY2;
    forces[5] = -b2*Y2 + b3*thetaZ2;

    forces[6] = -forces[0];
    forces[7] = -forces[1];
    forces[8] = -forces[2];

    forces[ 9] = a2*thetaX2;
    forces[10] = b2*Z2 + 2.f*b3*thetaY2;
    forces[11] = -b2*Y2 + 2.f*b3*thetaZ2;

    rotate_from_posX(axis, &forces[ 0], &forces[ 1], &forces[ 2]);
    rotate_from_posX(axis, &forces[ 3], &forces[ 4], &forces[ 5]);
    rotate_from_posX(axis, &forces[ 6], &forces[ 7], &forces[ 8]);
    rotate_from_posX(axis, &forces[ 9], &forces[10], &forces[11]);

    for (int k = 0; k < 12; k += 3)
    {
        float tmp[3];
        quaternion_rotate(&rot[i*4], &forces[k], tmp);
        forces[k] = tmp[0]; forces[k+1] = tmp[1]; forces[k+2] = tmp[2];
    }

    for (int k = 0; k < 12; k++) forces[k] *= -1.f;

    forces[0] -= damping_scaler1 * V12[0];
    forces[1] -= damping_scaler1 * V12[1];
    forces[2] -= damping_scaler1 * V12[2];

    forces[3] -= damping_scaler_rot1 * omega12[0];
    forces[4] -= damping_scaler_rot1 * omega12[1];
    forces[5] -= damping_scaler_rot1 * omega12[2];

    forces[6] -= damping_scaler2 * V21[0];
    forces[7] -= damping_scaler2 * V21[1];
    forces[8] -= damping_scaler2 * V21[2];

    forces[ 9] -= damping_scaler_rot2 * omega21[0];
    forces[10] -= damping_scaler_rot2 * omega21[1];
    forces[11] -= damping_scaler_rot2 * omega21[2];

    for (int k = 0; k < 3; k++)
    {
        acc[i3 + k] += forces[k];
        rot_acc[i3 + k] += forces[3 + k];

        acc[j3 + k] += forces[6 + k];
        rot_acc[j3 + k] += forces[9 + k];
    }
}
