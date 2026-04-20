#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"
#include "../Algos/utils.h"
#include "../Geom/quaternions.h"


void sim_step(config* C, float tau)
{
    int joints_count = C->joints_count;
    
    float* q = C->q->values;
    float* q_vel = C->q_vel->values;
    float* q_min = C->q_min->values;
    float* q_max = C->q_max->values;

    int q_vel_offset = 0;
    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = C->joints[i];
        joint_t* joint_data = (joint_t*) C->frames[joint_id]->data;
        int joint_type = joint_data->type;
        int q_id = joint_data->q_id;
        int q_vel_id = q_id + q_vel_offset;

        if (joint_type == 0 || joint_type == 1 || joint_type == 2)
        {
            q[q_id] += q_vel[q_vel_id] * tau;
            if (joint_data->has_limits)
            {
                q[q_id] = clip(q[q_id], q_min[q_id], q_max[q_id]);
            }
        }
        else if (joint_type == 3)
        {
            q[q_id    ] += q_vel[q_vel_id    ] * tau;
            q[q_id + 1] += q_vel[q_vel_id + 1] * tau;
            q[q_id + 2] += q_vel[q_vel_id + 2] * tau;

            float omega_x = q_vel[q_vel_id + 3];
            float omega_y = q_vel[q_vel_id + 4];
            float omega_z = q_vel[q_vel_id + 5];

            float angular_vel = sqrt(
                omega_x * omega_x +
                omega_y * omega_y +
                omega_z * omega_z
            );
            float theta = angular_vel * tau;

            float theta_half = theta * 0.5;
            float sin_theta = sinf(theta_half);
            float q_delta[4] = {
                cosf(theta_half),
                omega_x / angular_vel * sin_theta,
                omega_y / angular_vel * sin_theta,
                omega_z / angular_vel * sin_theta
            };

            float tmp[4];
            quaternion_product(q + q_id + 3, q_delta, tmp);

            q[q_id + 3] = tmp[0];
            q[q_id + 4] = tmp[1];
            q[q_id + 5] = tmp[2];
            q[q_id + 6] = tmp[3];

            q_vel_offset--;
        }
        #ifdef DEBUG
        else
        {
            printf("Invalid joint type!\n");
            exit(EXIT_FAILURE);
        }
        #endif
    }
    config_update_q(C);
}
