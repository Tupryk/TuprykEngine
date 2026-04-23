#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"
#include "../Algos/utils.h"
#include "../Physics/forces.h"
#include "../Geom/quaternions.h"


tensor* compute_acc_from_gravity(config* C)
{
    tensor* q_acc = tensor_copy(C->q_vel);
    float* q_acc_v = q_acc->values;

    int* joints = C->joints;
    int joints_count = C->joints_count;
    frame** frames = C->frames;
    tensor* gravity = C->gravity;

    tensor* c_r = new_tensor_vector(3, NULL);
    tensor* gravity_force = new_tensor_vector(3, NULL);
    tensor* torque = new_tensor_vector(3, NULL);
    tensor* angular_acc = new_tensor_vector(3, NULL);

    for (int i = 0; i < joints_count; i++)
    {
        frame* joint_frame = frames[joints[i]];
        joint_t* joint_data = (joint_t*) joint_frame->data;
        int joint_type = joint_data->type;
        int q_delta_id = joint_data->q_delta_id;

        if (joint_type == 0 || joint_type == 1 || joint_type == 2)  // Gravity on hinge
        {
            float axis[3] = {0.f, 0.f, 0.f};
            axis[joint_type] = 1.f;
            float axis_rotated[3];
            quaternion_rotate_point(joint_frame->rot->values, axis, axis_rotated);
    
            tensor_sub(joint_data->com, joint_frame->pos, c_r);
            tensor_scalar_mult(gravity, joint_data->total_mass, gravity_force);
            vector_cross(c_r, gravity_force, torque);

            tensor_mult(joint_data->I_cm_inv, torque, angular_acc);
            q_acc_v[q_delta_id] = angular_acc->values[joint_type] * axis_rotated[joint_type];
        }
        else if (joint_type == 3)  // Gravity on free
        {
            // q_acc_v[q_delta_id    ] = gravity->values[0];
            // q_acc_v[q_delta_id + 1] = gravity->values[1];
            // q_acc_v[q_delta_id + 2] = gravity->values[2];
        }
    }

    tensor_free(c_r);
    tensor_free(torque);
    tensor_free(gravity_force);
    tensor_free(angular_acc);

    return q_acc;
}

void q_acc_from_forces(config* C, force_t* f, tensor* q_acc)
{
    tensor* torque = new_tensor_vector(3, NULL);
    tensor* angular_acc = new_tensor_vector(3, NULL);
    tensor* c_r = new_tensor_vector(3, NULL);
    frame* from_frame = C->frames[f->frame_id];
    float* q_acc_v = q_acc->values;

    int index = f->frame_id;
    while(index != -1)
    {
        frame* current_frame = C->frames[index];

        if (current_frame->type == 4)
        {
            joint_t* joint_data = (joint_t*) current_frame->data;
            
            int joint_type = joint_data->type;
            int q_delta_id = joint_data->q_delta_id;
            
            float total_mass = joint_data->total_mass;
            tensor* com = joint_data->com;
            tensor* I_cm_inv = joint_data->I_cm_inv;
            tensor* force_world = f->force == NULL ? NULL : new_tensor_vector(3, NULL);
            tensor* torque_world = f->torque == NULL ? NULL : new_tensor_vector(3, NULL);
            tensor* poa_world = f->poa == NULL ? NULL : new_tensor_vector(3, NULL);

            if (force_world != NULL)
            {
                quaternion_rotate_point(
                    from_frame->rot->values,
                    f->force->values,
                    force_world->values
                );
            }
            // Does torque have to be rotated?
            if (torque_world != NULL)
            {
                tensor_transfer_values(torque_world, f->torque);
            }
            if (poa_world != NULL)
            {
                quaternion_rotate_point(
                    from_frame->rot->values,
                    f->poa->values,
                    poa_world->values
                );
                tensor_add(
                    from_frame->pos,
                    f->poa,
                    poa_world
                );
            }

            if (joint_type == 0 || joint_type == 1 || joint_type == 2)  // Hinge Joint
            {
                // Torque
                tensor_sub(poa_world, current_frame->pos, c_r);
                vector_cross(c_r, force_world, torque);
                
                // Friction
                tensor_scalar_sub(torque, joint_data->friction * C->q_vel->values[q_delta_id], torque);
                
                // Torque to Angular Acceleration
                tensor_mult(I_cm_inv, torque, angular_acc);

                // Apply to Hinge Axis
                float axis[3] = {0.f, 0.f, 0.f};
                float axis_rotated[3] = {0.f, 0.f, 0.f};
                axis[joint_type] = 1.f;
                quaternion_rotate_point(current_frame->rot->values, axis, axis_rotated);
                q_acc_v[q_delta_id] = angular_acc->values[joint_type] * axis_rotated[joint_type];
            }
            else if (joint_type == 3)  // Free Joint
            {
                // Force
                if (f->force != NULL)
                {
                    float* force = force_world->values;

                    // Apply Translational Acceleration Directly
                    q_acc_v[q_delta_id    ] += force[0] / total_mass;
                    q_acc_v[q_delta_id + 1] += force[1] / total_mass;
                    q_acc_v[q_delta_id + 2] += force[2] / total_mass;
                }

                // Torque
                if (poa_world != NULL)
                {
                    // Torque
                    tensor_sub(poa_world, com, c_r);
                    vector_cross(c_r, force_world, torque);
                }
                else tensor_fill(torque, 0.f);
                if (torque_world != NULL)
                {
                    tensor_add(torque, torque_world, torque);
                }

                tensor_mult(I_cm_inv, torque, angular_acc);

                q_acc_v[q_delta_id + 3] = angular_acc->values[0];
                q_acc_v[q_delta_id + 4] = angular_acc->values[1];
                q_acc_v[q_delta_id + 5] = angular_acc->values[2];
            }
            if (force_world != NULL) tensor_free(force_world);
            if (torque_world != NULL) tensor_free(torque_world);
            if (poa_world != NULL) tensor_free(poa_world);
        }
        index = current_frame->parent;
    }
    tensor_free(torque);
    tensor_free(angular_acc);
    tensor_free(c_r);
}

tensor* compute_acc_from_forces(config* C)
{
    tensor* q_acc = tensor_copy_shape(C->q_vel);
    tensor* q_acc_add = tensor_copy_shape(C->q_vel);

    struct stack_elem* current_contact_elem = C->forces->next;
    while (current_contact_elem != NULL)
    {
        q_acc_from_forces(C, current_contact_elem->data, q_acc_add);
        tensor_add(q_acc, q_acc_add, q_acc);
        current_contact_elem = current_contact_elem->next;
    }
    tensor_free(q_acc_add);

    return q_acc;
}

void step_vels(config* C, float tau)
{
    int* joints = C->joints;
    int joints_count = C->joints_count;

    float* q = C->q->values;
    float* q_min = C->q_min->values;
    float* q_max = C->q_max->values;
    float* q_vel = C->q_vel->values;

    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = joints[i];
        joint_t* joint_data = (joint_t*) C->frames[joint_id]->data;
        int joint_type = joint_data->type;
        int q_id = joint_data->q_id;
        int q_vel_id = joint_data->q_delta_id;

        if (joint_type == 0 || joint_type == 1 || joint_type == 2)
        {
            q[q_id] += q_vel[q_vel_id] * tau;
            if (joint_data->has_limits)
            {
                if (q[q_id] < q_min[q_id])
                {
                    q[q_id] = q_min[q_id];
                    q_vel[q_vel_id] = 0.f;
                }
                else if (q[q_id] > q_max[q_id])
                {
                    q[q_id] = q_max[q_id];
                    q_vel[q_vel_id] = 0.f;
                }
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

void sim_step(config* C, float tau)
{
    // Compute Joint COMs and Inertia Tensors
    config_populate_mass_inertias(C);

    // Update Velocities
    tensor* q_acc = compute_acc_from_forces(C);
    tensor* q_acc_g = compute_acc_from_gravity(C);
    tensor_add(q_acc, q_acc_g, q_acc);
    tensor_free(q_acc_g);

    tensor_scalar_mult(q_acc, tau, q_acc);
    tensor_add(C->q_vel, q_acc, C->q_vel);
    
    tensor_free(q_acc);
    
    // Apply Velocities
    step_vels(C, tau);

    // Compute collisions
    stack* contacts = config_get_contacts(C);
    
    // Compute impulses and update q
    int collision_iters = 10;
    for (int i = 0; i < collision_iters; i++)
    {
        struct stack_elem* current_contact_elem = contacts->next;
        while (current_contact_elem != NULL)
        {
            // TODO:
            current_contact_elem = current_contact_elem->next;
        }
    }
    contacts_free(contacts);

    // Apply Collision Adjusted Velocities
    step_vels(C, tau);
}
