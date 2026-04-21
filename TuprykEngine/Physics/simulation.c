#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"
#include "../Algos/utils.h"
#include "../Physics/forces.h"
#include "../Geom/quaternions.h"


void sim_step(config* C, float tau)
{
    int* joints = C->joints;
    int joints_count = C->joints_count;
    tensor* q_vel_ = C->q_vel;
    
    // Update Velocities
    int q_acc_offset = 0;
    tensor* q_acc = tensor_copy_shape(q_vel_);
    float* q_acc_v = q_acc->values;
    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = joints[i];
        frame* joint_frame = C->frames[joint_id];
        joint_t* joint_data = (joint_t*) joint_frame->data;
        
        int joint_type = joint_data->type;
        int q_id = joint_data->q_id;
        int q_acc_id = q_id + q_acc_offset;

        stack* all_forces = forces_on_body(C, joint_id);

        if (joint_type == 0 || joint_type == 1 || joint_type == 2)
        {
            force_t* current_force = (force_t*) stack_pop(all_forces);
            while (current_force != NULL)
            {
                tensor* c_r = tensor_sub_give(current_force->poa, joint_frame->pos);
                tensor* cross = vector_cross_give(c_r, current_force->force);
        
                // TODO: Rotate Hinge axis!!! + Inertia tensor
                q_acc_v[q_acc_id] += cross->values[joint_type];
        
                tensor_free(c_r);
                tensor_free(cross);
                
                force_free(current_force);
                current_force = (force_t*) stack_pop(all_forces);
            }
        }
        else if (joint_type == 3)
        {
            tensor* com = new_tensor_vector(3, NULL);
            float total_mass = center_of_mass(C, joint_id, com);

            force_t* current_force = (force_t*) stack_pop(all_forces);
            while (current_force != NULL)
            {
                // Force
                if (current_force->force != NULL)
                {
                    float* force = current_force->force->values;

                    q_acc_v[q_acc_id    ] += force[0];
                    q_acc_v[q_acc_id + 1] += force[1];
                    q_acc_v[q_acc_id + 2] += force[2];
                }

                // Torque
                if (current_force->poa != NULL)
                {
                    tensor* c_r = tensor_sub_give(current_force->poa, com);
                    tensor* cross = vector_cross_give(c_r, current_force->force);

                    q_acc_v[q_acc_id + 3] += cross->values[0];
                    q_acc_v[q_acc_id + 4] += cross->values[1];
                    q_acc_v[q_acc_id + 5] += cross->values[2];

                    tensor_free(c_r);
                    tensor_free(cross);
                }
                if (current_force->torque != NULL)
                {
                    float* torque = current_force->torque->values;
        
                    q_acc_v[q_acc_id + 3] += torque[0];
                    q_acc_v[q_acc_id + 4] += torque[1];
                    q_acc_v[q_acc_id + 5] += torque[2];
                }

                force_free(current_force);
                current_force = (force_t*) stack_pop(all_forces);
            }
            
            q_acc_v[q_acc_id    ] /= total_mass;
            q_acc_v[q_acc_id + 1] /= total_mass;
            q_acc_v[q_acc_id + 2] /= total_mass;
            // q_acc_v[q_acc_id + 2] -= 9.81f;
            
            q_acc_offset--;
            tensor_free(com);
        }

        stack_free(all_forces);
    }
    tensor_scalar_mult(q_acc, tau, q_acc);
    tensor_add(q_vel_, q_acc, q_vel_);
    tensor_free(q_acc);
    
    // Apply Velocities
    float* q = C->q->values;
    float* q_vel = q_vel_->values;
    float* q_min = C->q_min->values;
    float* q_max = C->q_max->values;

    int q_vel_offset = 0;
    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = joints[i];
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

            float q_delta[4];

            if (theta > 1e-6)
            {
                float theta_half = theta * 0.5;
                float sin_theta = sinf(theta_half);

                q_delta[0] = cosf(theta_half);
                q_delta[1] = omega_x / angular_vel * sin_theta;
                q_delta[2] = omega_y / angular_vel * sin_theta;
                q_delta[3] = omega_z / angular_vel * sin_theta;
            }
            else
            {
                q_delta[0] = 1.f;
                q_delta[1] = 0.f;
                q_delta[2] = 0.f;
                q_delta[3] = 0.f;
            }

            float tmp[4];
            quaternion_product(q + q_id + 3, q_delta, tmp);
            quaternion_normalize(tmp);

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
