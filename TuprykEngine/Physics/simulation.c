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

            q_acc_v[q_delta_id    ] = gravity->values[0];
            q_acc_v[q_delta_id + 1] = gravity->values[1];
            q_acc_v[q_delta_id + 2] = gravity->values[2];

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

void apply_q_delta(config* C, tensor* q_delta)
{
    float* q_delta_v = q_delta->values;
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
            q[q_id] += q_delta_v[q_vel_id];
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
            q[q_id    ] += q_delta_v[q_vel_id    ];
            q[q_id + 1] += q_delta_v[q_vel_id + 1];
            q[q_id + 2] += q_delta_v[q_vel_id + 2];

            float omega_x = q_delta_v[q_vel_id + 3];
            float omega_y = q_delta_v[q_vel_id + 4];
            float omega_z = q_delta_v[q_vel_id + 5];

            float angular_vel = sqrt(
                omega_x * omega_x +
                omega_y * omega_y +
                omega_z * omega_z
            );
            float theta = angular_vel;

            float q_omega[4];

            if (theta > 1e-6)
            {
                float theta_half = theta * 0.5;
                float sin_theta = sinf(theta_half);

                q_omega[0] = cosf(theta_half);
                q_omega[1] = omega_x / angular_vel * sin_theta;
                q_omega[2] = omega_y / angular_vel * sin_theta;
                q_omega[3] = omega_z / angular_vel * sin_theta;
            }
            else
            {
                q_omega[0] = 1.f;
                q_omega[1] = 0.f;
                q_omega[2] = 0.f;
                q_omega[3] = 0.f;
            }

            float tmp[4];
            quaternion_product(q + q_id + 3, q_omega, tmp);
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

void step_vels(config* C, float tau)
{
    tensor* q_delta = tensor_scalar_mult_give(C->q_vel, tau);
    apply_q_delta(C, q_delta);
    tensor_free(q_delta);
}

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

float inverse_effective_mass_from_body(config* C, int root_id, tensor* point, tensor* normal)
{
    frame* jf = C->frames[root_id];
    joint_t* jd = (joint_t*) jf->data;

    tensor* r = tensor_sub_give(point, jd->com);

    tensor* r_cross_n = vector_cross_give(r, normal);
    tensor* I_inv_term = tensor_mult_give(jd->I_cm_inv, r_cross_n);

    tensor* angular = vector_cross_give(r, I_inv_term);

    float k =
        (1.f / jd->total_mass)
        + vector_dot(angular, normal);

    tensor_free(r);
    tensor_free(r_cross_n);
    tensor_free(I_inv_term);
    tensor_free(angular);

    return k;
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

void load_collision_forces_to_joints(config* C)
{
    stack* contacts = config_get_contacts(C);
    float correction_strength = 10.f;
    float velocity_damper = 2.f;
    float beta = 0.2f;
    tensor* vel_a = new_tensor_vector(3, NULL);
    tensor* vel_b = new_tensor_vector(3, NULL);
    tensor* vel_rel = new_tensor_vector(3, NULL);

    struct stack_elem* current_contact_elem = contacts->next;
    while (current_contact_elem != NULL)
    {
        contact_t* contact = (contact_t*) current_contact_elem->data;

        velocity_at_point(C, contact->point, contact->body_a, vel_a);
        velocity_at_point(C, contact->point, contact->body_a, vel_b);
        tensor_sub(vel_a, vel_b, vel_rel);
        
        float vn = vector_dot(contact->normal, vel_rel);
        
        float penetration_force = correction_strength * (contact->depth + beta * vn);  // Baumgarte-style stabilization
        float damping_force = -velocity_damper * vn;

        float normal_mag = penetration_force + damping_force;
        if (normal_mag < 0.f) normal_mag = 0.f;
        
        tensor* contact_normal_a = tensor_scalar_mult_give(contact->normal, -1.f);
        tensor* contact_normal_b = tensor_scalar_mult_give(contact->normal,  1.f);

        tensor_scalar_mult(contact_normal_a, normal_mag, contact_normal_a);
        tensor_scalar_mult(contact_normal_b, normal_mag, contact_normal_b);

        impulse_to_joints_force(C, contact->body_a, contact_normal_a, contact->point);
        impulse_to_joints_force(C, contact->body_b, contact_normal_b, contact->point);
        
        tensor_free(contact_normal_a);
        tensor_free(contact_normal_b);
        current_contact_elem = current_contact_elem->next;
    }
    
    contacts_free(contacts);
    tensor_free(vel_a);
    tensor_free(vel_b);
    tensor_free(vel_rel);
}

void apply_velocity_impulse(config* C, int frame_id, tensor* normal, float correction, tensor* point, tensor* q_delta)
{
    frame** frames = C->frames;
    float* q_delta_v = q_delta->values;

    tensor* c_r = new_tensor_vector(3, NULL);
    tensor* omega = new_tensor_vector(3, NULL);
    tensor* torque = new_tensor_vector(3, NULL);
    tensor* corrected_normal = new_tensor_vector(3, NULL);

    int index = frame_id;
    while (index != -1)
    {
        frame* joint_frame = frames[index];
        if (joint_frame->type == 4)
        {
            joint_t* joint_data = (joint_t*) joint_frame->data;
            int joint_type = joint_data->type;
            int q_delta_id = joint_data->q_delta_id;
    
            if (joint_type == 0 || joint_type == 1 || joint_type == 2)  // Hinge Joint
            {
                tensor_sub(point, joint_frame->pos, c_r);
                tensor_scalar_mult(normal, correction, corrected_normal);
                vector_cross(c_r, corrected_normal, torque);
    
                // Torque to Angular Acceleration
                tensor_mult(joint_data->I_cm_inv, torque, omega);
    
                // Apply to Hinge Axis
                float axis[3] = {0.f, 0.f, 0.f};
                float axis_rotated[3] = {0.f, 0.f, 0.f};
                axis[joint_type] = 1.f;
                quaternion_rotate_point(joint_frame->rot->values, axis, axis_rotated);
                q_delta_v[q_delta_id] += omega->values[joint_type] * axis_rotated[joint_type];
            }
            else if (joint_type == 3)  // Free Joint
            {
                // Force
                float* normal_v = normal->values;
                float total_mass = joint_data->total_mass;
    
                q_delta_v[q_delta_id    ] += normal_v[0] * correction / total_mass;
                q_delta_v[q_delta_id + 1] += normal_v[1] * correction / total_mass;
                q_delta_v[q_delta_id + 2] += normal_v[2] * correction / total_mass;
    
                tensor_sub(point, joint_data->com, c_r);
                tensor_scalar_mult(normal, correction, corrected_normal);
                vector_cross(c_r, corrected_normal, torque);
    
                // Torque
                tensor_mult(joint_data->I_cm_inv, torque, omega);
    
                float* omega_v = omega->values;
                q_delta_v[q_delta_id + 3] += omega_v[0];
                q_delta_v[q_delta_id + 4] += omega_v[1];
                q_delta_v[q_delta_id + 5] += omega_v[2];
            }
        }
        index = joint_frame->parent;
    }

    tensor_free(c_r);
    tensor_free(omega);
    tensor_free(torque);
    tensor_free(corrected_normal);
}

tensor* solve_contact_velocity_constraints(config* C)
{
    tensor* q_delta = tensor_copy_shape(C->q_vel);
    stack* contacts = config_get_contacts(C);

    tensor* vel_a = new_tensor_vector(3, NULL);
    tensor* vel_b = new_tensor_vector(3, NULL);
    tensor* vel_rel = new_tensor_vector(3, NULL);

    struct stack_elem* current_contact_elem = contacts->next;
    while (current_contact_elem != NULL)
    {
        contact_t* contact = (contact_t*) current_contact_elem->data;

        velocity_at_point(C, contact->point, contact->body_a, vel_a);
        velocity_at_point(C, contact->point, contact->body_b, vel_b);
        tensor_sub(vel_a, vel_b, vel_rel);

        float vn = vector_dot(vel_rel, contact->normal);

        if (vn < 0)
        {
            float correction = -vn;

            apply_velocity_impulse(
                C,
                contact->body_a,
                contact->normal,
                correction,
                contact->point,
                q_delta
            );

            tensor* neg_normal = tensor_scalar_mult_give(contact->normal, -1.f);
            apply_velocity_impulse(
                C,
                contact->body_b,
                neg_normal,
                correction,
                contact->point,
                q_delta
            );
            tensor_free(neg_normal);
        }

        current_contact_elem = current_contact_elem->next;
    }
    
    tensor_free(vel_a);
    tensor_free(vel_b);
    tensor_free(vel_rel);
    contacts_free(contacts);
    return q_delta;
}

void sim_step(config* C, float tau)
{
    // Compute Joint COMs and Inertia Tensors
    config_populate_mass_inertias(C);
    config_empty_joints_accumulated_forces(C);

    // Update Accelerations
    load_forces_to_joints(C);
    load_collision_forces_to_joints(C);
    tensor* q_acc = q_delta_from_forces(C);
    tensor* q_acc_g = q_acc_from_gravity(C);
    // tensor_add(q_acc, q_acc_g, q_acc);
    tensor_free(q_acc_g);
    
    // Integrate Velocites and Positions
    tensor_scalar_mult(q_acc, tau, q_acc);
    tensor_add(C->q_vel, q_acc, C->q_vel);
    // tensor* q_delta = solve_contact_velocity_constraints(C);
    // apply_q_delta(C, q_delta);
    // tensor_free(q_delta);
    step_vels(C, tau);

    tensor_free(q_acc);
}
