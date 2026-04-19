#include <stdio.h>
#include <stdlib.h>

#include "simulation.h"
#include "../Algos/utils.h"


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
            q[q_id] += q_vel[q_vel_id];
            if (joint_data->has_limits)
            {
                q[q_id] = clip(q[q_id], q_min[q_id], q_max[q_id]);
            }
        }
        else if (joint_type == 3)
        {
            q[q_id    ] += q_vel[q_vel_id    ];
            q[q_id + 1] += q_vel[q_vel_id + 1];
            q[q_id + 2] += q_vel[q_vel_id + 2];
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
