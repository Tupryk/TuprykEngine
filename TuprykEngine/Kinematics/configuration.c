#include <stdio.h>
#include <stdlib.h>

#include "configuration.h"
#include "../Geom/quaternions.h"
#include "../visual/prints/linalg.h"


int config_colliding(config* C)
{
    // TODO: KD_Tree
    for (int i = 0; i < C->frame_count; i++)
    {
        frame* a = C->frames[i];
        if (a->type == 1)
        {
            float ar = *(float*) a->data;
            for (int j = i+1; j < C->frame_count; j++)
            {
                frame* b = C->frames[j];
                if (b->type == 1)
                {
                    float br = *(float*) b->data;

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
    free(C);
}
