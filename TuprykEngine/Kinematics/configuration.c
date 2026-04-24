#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"
#include "../Physics/forces.h"
#include "../Geom/quaternions.h"
#include "../visual/prints/linalg.h"


int config_colliding(config* C)
{
    // TODO: KD_Tree
    frame** frames = C->frames;
    int frame_count = C->frame_count;
    for (int i = 0; i < frame_count; i++)
    {
        frame* a = frames[i];
        if (a->type == 1)
        {
            geom* a_geom_data = (geom*) a->data;
            float ar = *(float*) a_geom_data->mesh;
            for (int j = i+1; j < frame_count; j++)
            {
                frame* b = frames[j];
                if (b->type == 1)
                {
                    geom* b_geom_data = (geom*) b->data;
                    float br = *(float*) b_geom_data->mesh;

                    tensor* diff = tensor_copy(b->pos);
                    tensor_sub(a->pos, diff, diff);

                    float dist = vector_norm(diff);
                    if (dist < ar || dist < br)
                    {
                        return 1;
                    }
                    tensor_free(diff);
                }
            }
        }
    }
    return 0;
}

void velocity_at_point(config* C, tensor* point, int frame_id, tensor* vel)
{
    int index = frame_id;
    float* q_vel = C->q_vel->values;
    tensor* new_vel = new_tensor_vector(3, NULL);
    tensor* c_r = new_tensor_vector(3, NULL);
    while (index != -1)
    {
        frame* current_frame = C->frames[index];
        if (current_frame->type == 4)
        {
            joint_t* joint_data = (joint_t*) current_frame->data;
            int joint_type = joint_data->type;
            int q_delta_id = joint_data->q_delta_id;

            if (joint_type == 0 || joint_type == 1 || joint_type == 2)
            {
                float axis[3] = {0.f, 0.f, 0.f};
                float axis_world[3] = {0.f, 0.f, 0.f};
                axis[joint_type] = 1.f;
                quaternion_rotate_point(current_frame->rot->values, axis, axis_world);

                tensor_sub(point, current_frame->pos, c_r);

                tensor* omega = new_tensor_vector(3, NULL);
                omega->values[0] = q_vel[q_delta_id] * axis_world[0];
                omega->values[1] = q_vel[q_delta_id] * axis_world[1];
                omega->values[2] = q_vel[q_delta_id] * axis_world[2];

                vector_cross(omega, c_r, new_vel);
            }
            else if (joint_type == 3)
            {
                tensor_sub(point, joint_data->com, c_r);
                
                tensor* omega = new_tensor_vector(3, NULL);
                omega->values[0] = q_vel[q_delta_id + 3];
                omega->values[1] = q_vel[q_delta_id + 4];
                omega->values[2] = q_vel[q_delta_id + 5];

                vector_cross(omega, c_r, new_vel);
                new_vel->values[0] += q_vel[q_delta_id    ];
                new_vel->values[1] += q_vel[q_delta_id + 1];
                new_vel->values[2] += q_vel[q_delta_id + 2];
                
                tensor_free(omega);
            }
            tensor_add(vel, new_vel, vel);
        }
        index = current_frame->parent;
    }
    tensor_free(new_vel);
    tensor_free(c_r);
}

stack* config_get_contacts(config* C)
{
    // TODO: KD_Tree
    stack* contacts = stack_init();
    frame** frames = C->frames;
    int frame_count = C->frame_count;
    for (int i = 0; i < frame_count; i++)
    {
        frame* a = frames[i];
        if (a->type == 1)
        {
            geom* a_geom_data = (geom*) a->data;
            float ar = *(float*) a_geom_data->mesh;
            for (int j = i+1; j < frame_count; j++)
            {
                frame* b = frames[j];
                if (b->type == 1)
                {
                    geom* b_geom_data = (geom*) b->data;
                    float br = *(float*) b_geom_data->mesh;

                    tensor* diff = tensor_copy(b->pos);
                    tensor_sub(a->pos, diff, diff);

                    float dist = vector_norm(diff);
                    if (dist < ar || dist < br)
                    {
                        contact_t* contact = (contact_t*) malloc(sizeof(contact_t));

                        contact->body_a = i;
                        contact->body_b = j;
                        
                        contact->point = tensor_add_give(a->pos, b->pos);
                        tensor_scalar_mult(contact->point, 0.5f, contact->point);
                        
                        contact->normal = tensor_sub_give(b->pos, a->pos);
                        vector_normalize(contact->normal);
                        
                        contact->depth = ar + br - dist;
                        contact->accumulated_impulse = 0.f;

                        stack_push(contacts, contact);
                    }
                    tensor_free(diff);
                }
            }
        }
    }
    return contacts;
}

void config_populate_mass_inertias(config* C)
{
    int* joints = C->joints;
    int joints_count = C->joints_count;

    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = joints[i];
        frame* joint_frame = C->frames[joint_id];
        joint_t* joint_data = (joint_t*) joint_frame->data;

        if (joint_data->com == NULL) joint_data->com = new_tensor_vector(3, NULL);
        joint_data->total_mass = center_of_mass(C, joint_id, joint_data->com);
        
        if (joint_data->I_cm_inv == NULL) joint_data->I_cm_inv = new_tensor_matrix(3, 3, NULL);
        
        tensor* I_cm = new_tensor_matrix(3, 3, NULL);
        combined_inertia(C, joint_id, joint_data->com, I_cm);
        tensor_inverse(I_cm, joint_data->I_cm_inv);
        tensor_free(I_cm);
    }
}

void config_empty_joints_accumulated_forces(config* C)
{
    int* joints = C->joints;
    int joints_count = C->joints_count;
    frame** frames = C->frames;

    for (int i = 0; i < joints_count; i++)
    {
        frame* joint_frame = frames[joints[i]];
        joint_t* joint_data = (joint_t*) joint_frame->data;

        if (joint_data->accumulated_forces == NULL)
        {
            joint_data->accumulated_forces = new_tensor_vector(3, NULL);
        }
        else
        {
            tensor_fill(joint_data->accumulated_forces, 0.f);
        }

        if (joint_data->accumulated_torques == NULL)
        {
            joint_data->accumulated_torques = new_tensor_vector(3, NULL);
        }
        else
        {
            tensor_fill(joint_data->accumulated_torques, 0.f);
        }
    }
}

void impulse_to_joint_force(frame* joint_frame, tensor* impulse_world, tensor* poa_world)
{
    joint_t* joint_data = (joint_t*) joint_frame->data;
    
    int joint_type = joint_data->type;
    int q_delta_id = joint_data->q_delta_id;
    
    tensor* torque = new_tensor_vector(3, NULL);
    tensor* c_r = new_tensor_vector(3, NULL);

    if (joint_type == 0 || joint_type == 1 || joint_type == 2)  // Hinge Joint
    {
        // Torque
        tensor_sub(poa_world, joint_frame->pos, c_r);
        vector_cross(c_r, impulse_world, torque);
        tensor_add(torque, joint_data->accumulated_torques, joint_data->accumulated_torques);
    }
    else if (joint_type == 3)  // Free Joint
    {
        // Force
        tensor_add(impulse_world, joint_data->accumulated_forces, joint_data->accumulated_forces);

        // Torque
        tensor_sub(poa_world, joint_data->com, c_r);
        vector_cross(c_r, impulse_world, torque);
        tensor_add(torque, joint_data->accumulated_torques, joint_data->accumulated_torques);
    }

    tensor_free(torque);
    tensor_free(c_r);
}

void impulse_to_joints_force(config* C, int from_frame_id, tensor* impulse_world, tensor* poa_world)
{
    int index = from_frame_id;
    while(index != -1)
    {
        frame* current_frame = C->frames[index];

        if (current_frame->type == 4)
        {
            impulse_to_joint_force(current_frame, impulse_world, poa_world);
        }
        index = current_frame->parent;
    }
}

void contacts_free(stack* s)
{
    while (s->next != NULL)
    {
        contact_t* contact = stack_pop(s);
        tensor_free(contact->point);
        tensor_free(contact->normal);
        free(contact);
    }
    free(s);
}

void config_set_q(config* C, float* q)
{
    int q_dim = C->q->volume;
    for (int i = 0; i < q_dim; i++)
    {
        C->q->values[i] = q[i];
    }
    config_update_q(C);
}

void config_update_frame_pose(config* C, frame* f)
{
    if (f->parent != -1)
    {
        frame* parent = C->frames[f->parent];
        quaternion_product(parent->rot->values, f->rot_rel->values, f->rot->values);

        quaternion_rotate_point(parent->rot->values, f->pos_rel->values, f->pos->values);
        tensor_add(parent->pos, f->pos, f->pos);
    }
    for (int i = 0; i < f->children_count; i++)
    {
        int child_id = f->children[i];
        config_update_frame_pose(C, C->frames[child_id]);
    }
}

void config_update_q(config* C)
{
    // Update relative poses based on q
    int joints_count = C->joints_count;
    for (int i = 0; i < joints_count; i++)
    {
        int joint_id = C->joints[i];
        frame* joint_frame = C->frames[joint_id];
        joint_t* joint_data = (joint_t*) joint_frame->data;
        int joint_type = joint_data->type;
        
        int q_id = joint_data->q_id;
        float* qs = C->q->values;
        float phi = qs[q_id];
        float* pos_rel = joint_frame->pos_rel->values;
        float* rot_rel = joint_frame->rot_rel->values;

        if (joint_type == 0)
        {
            quaternion_x(phi, rot_rel);
        }
        else if (joint_type == 1)
        {
            quaternion_y(phi, rot_rel);
        }
        else if (joint_type == 2)
        {
            quaternion_z(phi, rot_rel);
        }
        else if (joint_type == 3)
        {
            pos_rel[0] = qs[q_id    ];
            pos_rel[1] = qs[q_id + 1];
            pos_rel[2] = qs[q_id + 2];

            rot_rel[0] = qs[q_id + 3];
            rot_rel[1] = qs[q_id + 4];
            rot_rel[2] = qs[q_id + 5];
            rot_rel[3] = qs[q_id + 6];

            quaternion_normalize(rot_rel);
        }
        #ifdef DEBUG
        else
        {
            printf("Unknown joint type %d!\n", joint_frame->type);
            exit(EXIT_FAILURE);
        }
        #endif
    }

    // Update universal poses based on relative poses
    config_update_frame_pose(C, C->frames[0]);
}

void config_free(config* C)
{
    if (C->frame_count)
    {
        for (int i = 0; i < C->frame_count; i++)
        {
            frame_free(C->frames[i]);
        }
        free(C->frames);
    }
    if (C->joints != NULL)
    {
        free(C->joints);
    }
    if (C->lights != NULL)
    {
        free(C->lights);
    }
    if (C->q != NULL)
    {
        tensor_free(C->q);
        tensor_free(C->q_max);
        tensor_free(C->q_min);
    }
    if (C->q_vel != NULL)
    {
        tensor_free(C->q_vel);
    }
    if (C->forces != NULL)
    {
        while (C->forces->size > 0)
        {
            force_t* F = (force_t*) stack_pop(C->forces);
            force_free(F);
        }
        stack_free(C->forces);
    }
    free(C);
}

void force_free(force_t* f)
{
    if (f->force != NULL) tensor_free(f->force);
    if (f->torque != NULL) tensor_free(f->torque);
    if (f->poa != NULL) tensor_free(f->poa);
    free(f);
}

void forces_add(force_t* a, force_t* b, force_t* out)
{
    if (a->force != NULL && b->force != NULL)
    {
        tensor_add(a->force, b->force, out->force);
    }
    if (a->torque != NULL && b->torque != NULL)
    {
        tensor_add(a->torque, b->torque, out->torque);
    }
    // TODO: poa?
}

int root_joint(config* C, int frame_id)
{
    int index = frame_id;
    int last_joint_id = -1;
    while (index != -1)
    {
        frame* current_frame = C->frames[index];
        if (current_frame->type == 4)
        {
            last_joint_id = index;
        }
        index = current_frame->parent;
    }
    return last_joint_id;
}
