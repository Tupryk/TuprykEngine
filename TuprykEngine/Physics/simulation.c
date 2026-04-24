#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "simulation.h"
#include "../Algos/utils.h"
#include "../Physics/forces.h"
#include "../Geom/quaternions.h"

#include "../visual/prints/linalg.h"


tensor* q_acc_from_gravity(config* C)
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
            tensor_sub(joint_data->com, joint_frame->pos, c_r);
            tensor_scalar_mult(gravity, joint_data->total_mass, gravity_force);
            vector_cross(c_r, gravity_force, torque);
            
            // // Friction
            // tensor_scalar_sub(torque, joint_data->friction * C->q_vel->values[q_delta_id], torque);
            
            tensor_mult(joint_data->I_cm_inv, torque, angular_acc);
            
            float axis[3] = {0.f, 0.f, 0.f};
            axis[joint_type] = 1.f;
            float axis_rotated[3];
            quaternion_rotate_point(joint_frame->rot->values, axis, axis_rotated);

            q_acc_v[q_delta_id] = angular_acc->values[joint_type] * axis_rotated[joint_type];
        }
        else if (joint_type == 3)  // Gravity on free
        {
            // TODO: Add air friction here?

            // q_acc_v[q_delta_id    ] = gravity->values[0];
            // q_acc_v[q_delta_id + 1] = gravity->values[1];
            // q_acc_v[q_delta_id + 2] = gravity->values[2];

            tensor_sub(joint_data->com, joint_frame->pos, c_r);
            tensor_scalar_mult(gravity, joint_data->total_mass, gravity_force);
            vector_cross(c_r, gravity_force, torque);

            // // Friction
            // torque->values[0] -= 0.1f * C->q_vel->values[q_delta_id + 3];
            // torque->values[1] -= 0.1f * C->q_vel->values[q_delta_id + 4];
            // torque->values[2] -= 0.1f * C->q_vel->values[q_delta_id + 5];

            tensor_mult(joint_data->I_cm_inv, torque, angular_acc);
            q_acc_v[q_delta_id + 3] = angular_acc->values[0];
            q_acc_v[q_delta_id + 4] = angular_acc->values[1];
            q_acc_v[q_delta_id + 5] = angular_acc->values[2];
        }
    }

    tensor_free(c_r);
    tensor_free(torque);
    tensor_free(gravity_force);
    tensor_free(angular_acc);

    return q_acc;
}

// tensor* q_acc_from_forces(config* C)
// {
//     tensor* q_acc = tensor_copy_shape(C->q_vel);
//     tensor* q_acc_add = tensor_copy_shape(C->q_vel);

//     struct stack_elem* current_contact_elem = C->forces->next;
//     while (current_contact_elem != NULL)
//     {
//         force_t* f = (force_t*) current_contact_elem->data;

//         frame* from_frame = C->frames[f->frame_id];

//         tensor* force_world = f->force == NULL ? NULL : new_tensor_vector(3, NULL);
//         tensor* poa_world = f->poa == NULL ? NULL : new_tensor_vector(3, NULL);

//         if (force_world != NULL)
//         {
//             quaternion_rotate_point(from_frame->rot->values, f->force->values, force_world->values);
//         }
//         if (poa_world != NULL)
//         {
//             quaternion_rotate_point(from_frame->rot->values, f->poa->values, poa_world->values);
//             tensor_add(from_frame->pos, f->poa, poa_world);
//         }

//         config_q_delta_from_impulse(C, f->frame_id, force_world, poa_world, q_acc_add);
//         tensor_add(q_acc, q_acc_add, q_acc);

//         if (force_world != NULL) tensor_free(force_world);
//         if (poa_world != NULL) tensor_free(poa_world);
//         current_contact_elem = current_contact_elem->next;
//     }
//     tensor_free(q_acc_add);

//     return q_acc;
// }

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
            // if (joint_data->has_limits)
            // {
            //     if (q[q_id] < q_min[q_id])
            //     {
            //         q[q_id] = q_min[q_id];
            //         q_vel[q_vel_id] = 0.f;
            //     }
            //     else if (q[q_id] > q_max[q_id])
            //     {
            //         q[q_id] = q_max[q_id];
            //         q_vel[q_vel_id] = 0.f;
            //     }
            // }
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

// void config_apply_force()
// {
//     // Linear
//     q_vel[q_vel_id_a    ] += impulse[0] / mass_a;
//     q_vel[q_vel_id_a + 1] += impulse[1] / mass_a;
//     q_vel[q_vel_id_a + 2] += impulse[2] / mass_a;

//     // Angular
//     float delta_omega_a[3];
//     cross(r_a, impulse, delta_omega_a);
//     mat3_vec_mult(I_cm_inv_a, delta_omega_a, delta_omega_a);
//     q_vel[q_vel_id_a + 3] += delta_omega_a[0];
//     q_vel[q_vel_id_a + 4] += delta_omega_a[1];
//     q_vel[q_vel_id_a + 5] += delta_omega_a[2];
// }

void load_forces_to_joints(config* C)
{
    struct stack_elem* current_contact_elem = C->forces->next;
    while (current_contact_elem != NULL)
    {
        force_t* f = (force_t*) current_contact_elem->data;

        frame* from_frame = C->frames[f->frame_id];

        tensor* force_world = f->force == NULL ? NULL : new_tensor_vector(3, NULL);
        tensor* poa_world = f->poa == NULL ? NULL : new_tensor_vector(3, NULL);

        if (force_world != NULL)
        {
            quaternion_rotate_point(from_frame->rot->values, f->force->values, force_world->values);
        }
        if (poa_world != NULL)
        {
            quaternion_rotate_point(from_frame->rot->values, f->poa->values, poa_world->values);
            tensor_add(from_frame->pos, f->poa, poa_world);
        }

        impulse_to_joints_force(C, f->frame_id, force_world, poa_world);

        if (force_world != NULL) tensor_free(force_world);
        if (poa_world != NULL) tensor_free(poa_world);
        current_contact_elem = current_contact_elem->next;
    }
}

tensor* q_delta_from_forces(config* C)
{
    tensor* q_acc = tensor_copy_shape(C->q_vel);
    float* q_acc_v = q_acc->values;

    int* joints = C->joints;
    int joints_count = C->joints_count;
    frame** frames = C->frames;

    tensor* angular_acc = new_tensor_vector(3, NULL);

    for (int i = 0; i < joints_count; i++)
    {
        frame* joint_frame = frames[joints[i]];
        joint_t* joint_data = (joint_t*) joint_frame->data;
        int joint_type = joint_data->type;
        int q_delta_id = joint_data->q_delta_id;

        if (joint_type == 0 || joint_type == 1 || joint_type == 2)  // Hinge Joint
        {
            // Torque to Angular Acceleration
            tensor_mult(joint_data->I_cm_inv, joint_data->accumulated_torques, angular_acc);

            // Apply to Hinge Axis
            float axis[3] = {0.f, 0.f, 0.f};
            float axis_rotated[3] = {0.f, 0.f, 0.f};
            axis[joint_type] = 1.f;
            quaternion_rotate_point(joint_frame->rot->values, axis, axis_rotated);
            q_acc_v[q_delta_id] = angular_acc->values[joint_type] * axis_rotated[joint_type];
        }
        else if (joint_type == 3)  // Free Joint
        {
            // Force
            float* force = joint_data->accumulated_forces->values;
            float total_mass = joint_data->total_mass;

            q_acc_v[q_delta_id    ] = force[0] / total_mass;
            q_acc_v[q_delta_id + 1] = force[1] / total_mass;
            q_acc_v[q_delta_id + 2] = force[2] / total_mass;

            // Torque
            tensor_mult(joint_data->I_cm_inv, joint_data->accumulated_torques, angular_acc);

            float* angular_acc_v = angular_acc->values;
            q_acc_v[q_delta_id + 3] = angular_acc_v[0];
            q_acc_v[q_delta_id + 4] = angular_acc_v[1];
            q_acc_v[q_delta_id + 5] = angular_acc_v[2];
        }
    }
    tensor_free(angular_acc);
    return q_acc;
}

void sim_step(config* C, float tau)
{
    // Compute Joint COMs and Inertia Tensors
    config_populate_mass_inertias(C);
    config_empty_joints_accumulated_forces(C);

    // Update Velocities
    load_forces_to_joints(C);
    tensor* q_acc = q_delta_from_forces(C);
    tensor* q_acc_g = q_acc_from_gravity(C);
    // tensor_add(q_acc, q_acc_g, q_acc);
    tensor_free(q_acc_g);
    
    tensor_scalar_mult(q_acc, tau, q_acc);
    tensor_add(C->q_vel, q_acc, C->q_vel);
    
    tensor_free(q_acc);
    
    // Apply Velocities
    tensor* q_copy = tensor_copy(C->q);
    step_vels(C, tau);
    
    // Compute collisions
    stack* contacts = config_get_contacts(C);
    
    // Compute impulses and update q
    float beta = 0.1f;              // Correction strength, tune between 0.1 - 0.3
    float slop = 0.001f;            // Don't correct tiny penetrations (avoids jitter)
    float restitution_coeff = 0.f;  // Bouncyness, 0 -> no bounce 1 -> full bounce
    int collision_iters = 1;
    
    config_populate_mass_inertias(C);
    tensor* vel_a = new_tensor_vector(3, NULL);
    tensor* vel_b = new_tensor_vector(3, NULL);
    tensor* vel_rel = new_tensor_vector(3, NULL);
    tensor* neg_normal = new_tensor_vector(3, NULL);
    tensor* impulse_a = new_tensor_vector(3, NULL);
    tensor* impulse_b = new_tensor_vector(3, NULL);

    for (int i = 0; i < collision_iters; i++)
    {
        struct stack_elem* current_contact_elem = contacts->next;
        while (current_contact_elem != NULL)
        {
            contact_t* contact = (contact_t*) current_contact_elem->data;

            // Relative velocities at contact points
            velocity_at_point(C, contact->point, contact->body_a, vel_a);
            velocity_at_point(C, contact->point, contact->body_a, vel_b);
            tensor_sub(vel_a, vel_b, vel_rel);
            float vel_rel_normal = vector_dot(vel_rel, contact->normal);

            // Effective Mass
            float effective_mass = 0.f;

            int a_root_joint_id = root_joint(C, contact->body_a);
            if (a_root_joint_id != -1)
            {
                frame* jf = C->frames[a_root_joint_id];
                joint_t* jd = (joint_t*) jf->data;

                tensor* r_a = new_tensor_vector(3, NULL);
                if (jd->type == 0 || jd->type == 1 || jd->type == 2)
                {
                    tensor_sub(contact->point, jf->pos, r_a);
                }
                else if (jd->type == 3)
                {
                    tensor_sub(contact->point, jd->com, r_a);
                }
                tensor* r_a_cross_n = vector_cross_give(r_a, contact->normal);
                tensor* I_inv_r_a = tensor_mult_give(jd->I_cm_inv, r_a_cross_n);
                tensor* r_a_term = vector_cross_give(I_inv_r_a, r_a);
                
                effective_mass += (1.f / jd->total_mass) + vector_dot(r_a_term, contact->normal);

                tensor_free(r_a_cross_n);
                tensor_free(I_inv_r_a);
                tensor_free(r_a_term);
                tensor_free(r_a);
            }

            int b_root_joint_id = root_joint(C, contact->body_b);
            if (b_root_joint_id != -1)
            {
                frame* jf = C->frames[b_root_joint_id];
                joint_t* jd = (joint_t*) jf->data;

                tensor* r_a = new_tensor_vector(3, NULL);
                if (jd->type == 0 || jd->type == 1 || jd->type == 2)
                {
                    tensor_sub(contact->point, jf->pos, r_a);
                }
                else if (jd->type == 3)
                {
                    tensor_sub(contact->point, jd->com, r_a);
                }
                tensor* r_a_cross_n = vector_cross_give(r_a, contact->normal);
                tensor* I_inv_r_a = tensor_mult_give(jd->I_cm_inv, r_a_cross_n);
                tensor* r_a_term = vector_cross_give(I_inv_r_a, r_a);
                
                effective_mass += (1.f / jd->total_mass) + vector_dot(r_a_term, contact->normal);

                tensor_free(r_a_cross_n);
                tensor_free(I_inv_r_a);
                tensor_free(r_a_term);
                tensor_free(r_a);
            }

            // Impulse Magnitude
            float bias = (beta / tau) * fmaxf(0.f, contact->depth - slop);
            float delta_j = (-(1.f + restitution_coeff) * vel_rel_normal + bias) / effective_mass;
            float new_impulse = fmaxf(contact->accumulated_impulse + delta_j, 0.f);
            float actual_delta = new_impulse - contact->accumulated_impulse;
            contact->accumulated_impulse = new_impulse;

            // Impulse to Velocities
            float impulse_a_values[3] = {
                contact->normal->values[0] * actual_delta,
                contact->normal->values[1] * actual_delta,
                contact->normal->values[2] * actual_delta,
            };
            tensor_set_values(impulse_a, impulse_a_values);
            
            float impulse_b_values[3] = {
                -impulse_a_values[0],
                -impulse_a_values[1],
                -impulse_a_values[2]
            };
            tensor_set_values(impulse_b, impulse_b_values);

            impulse_to_joints_force(C, contact->body_a, impulse_a, contact->point);
            impulse_to_joints_force(C, contact->body_b, impulse_b, contact->point);

            tensor* q_delta = q_delta_from_forces(C);
            tensor_add(q_delta, C->q_vel, C->q_vel);
            tensor_free(q_delta);
            
            current_contact_elem = current_contact_elem->next;
        }
    }
    
    tensor_free(vel_a);
    tensor_free(vel_b);
    tensor_free(vel_rel);
    tensor_free(neg_normal);
    tensor_free(impulse_a);
    tensor_free(impulse_b);
    contacts_free(contacts);

    // Apply Collision Adjusted Velocities (TODO: Is this correct?)
    config_set_q(C, q_copy->values);
    step_vels(C, tau);
    tensor_free(q_copy);
}
