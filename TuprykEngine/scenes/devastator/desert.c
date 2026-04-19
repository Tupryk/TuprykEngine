#include <stdio.h>
#include <stdlib.h>

#include "desert.h"
#include "../../Stochastic/sample.h"


geom* ball_geom_give(float radius)
{
    geom* ball_geom = (geom*) malloc(sizeof(geom));
    
    ball_geom->type = 0;
    
    ball_geom->mesh = (void*) malloc(sizeof(float));
    *(float*)ball_geom->mesh = radius;
    ball_geom->mass = radius;

    ball_geom->tex = (texture*) malloc(sizeof(texture));

    ball_geom->tex->color[0] = 1.0f;
    ball_geom->tex->color[1] = 0.4f;
    ball_geom->tex->color[2] = 0.f;

    ball_geom->tex->ambient   = 0.5;
    ball_geom->tex->diffuse   = 1.f;
    ball_geom->tex->specular  = 0.5f;
    ball_geom->tex->shininess = 10.f;

    return ball_geom;
}

void add_tentacle_piece_to_config(
    config* C,
    float* pos_values,
    int joint_idx,
    int parent_idx,
    int is_leaf,
    int q_id,
    int j, int i
) {
    tensor* parent_pos = C->frames[parent_idx]->pos;
    float rot[] = {1.f, 0.f, 0.f, 0.f};
    int ball_idx = joint_idx + 1;

    // Poses and parents
    frame* joint = frame_init(parent_pos->values, rot);
    joint->parent = parent_idx;

    frame* ball = frame_init(pos_values, rot);
    ball->parent = joint_idx;

    // Relavtive poses
    joint->pos_rel = tensor_sub_give(joint->pos, parent_pos);
    if (j != -1)
    {
        // float rot_joint[] = {0.92388f, 0.38168f, 0.f, 0.f};
        // float rot_joint[] = {0.92388f, -0.38168f, 0.f, 0.f};
        // float rot_joint[] = {0.92388f, 0.f, 0.38168f, 0.f};
        float rot_joint[] = {
            // 0.92388f, 0.38168f, 0.f, 0.f   //  45 X
            // 0.92388f, -0.38168f, 0.f, 0.f  // -45 X
            0.92388f, 0.f, 0.38168f, 0.f   //  45 Y
            // 0.92388f, 0.f, -0.38168f, 0.f  // -45 Y
            // 0.92388f, 0.f, 0.f, 0.38168f   //  45 Z
            // 0.92388f, 0.f, 0.f, -0.38168f  // -45 Z
        };
        // float rot_joint[] = {0.92388f, 0.f, 0.f, 0.38168f};
        // float rot_joint[] = {0.92388f, 0.f, 0.f, -0.38168f};
        int rot_shape[] = {4, 1};
        joint->rot_rel = new_tensor(rot_shape, 2, rot_joint);
    }
    else
    {
        joint->rot_rel = tensor_copy(ball->rot);
    }
    ball->pos_rel  = tensor_sub_give(ball->pos, joint->pos);
    ball->rot_rel  = tensor_copy(ball->rot);
    
    // Children
    joint->children_count = 1;
    joint->children = (int*) malloc(sizeof(int));
    joint->children[0] = ball_idx;

    if (is_leaf)
    {
        ball->children_count = 0;
        ball->children = NULL;
    }
    else
    {
        ball->children_count = 1;
        ball->children = (int*) malloc(sizeof(int));
        ball->children[0] = ball_idx + 1;
    }
    
    // Fill data
    joint_t* joint_data = (joint_t*) malloc(sizeof(joint_t));
    joint_data->type = i % 2 ? 0 : 1;
    joint_data->has_limits = 1;
    joint_data->q_id = q_id;
    joint->data = (void*) joint_data;
    
    geom* ball_geom = ball_geom_give(0.1f);
    switch (j)
    {
        case 0:
            ball_geom->tex->color[0] = 0.f;
            ball_geom->tex->color[1] = 1.f;
            ball_geom->tex->color[2] = 0.f;
            break;
        case 1:
            ball_geom->tex->color[0] = 0.f;
            ball_geom->tex->color[1] = 0.f;
            ball_geom->tex->color[2] = 1.f;
            break;
        case 2:
            ball_geom->tex->color[0] = 1.f;
            ball_geom->tex->color[1] = 0.f;
            ball_geom->tex->color[2] = 0.f;
            break;
    }
    ball->data = (void*) ball_geom;
    
    // Set types
    joint->type = 4;
    ball->type = 1;
    
    // Add to config
    C->frames[joint_idx] = joint;
    C->frames[ball_idx] = ball;
}

config* init_devastator_config()
{
    float origin_pos[] = {0.f, 0.f, 0.f};
    float origin_rot[] = {1.f, 0.f, 0.f, 0.f};

    config* C = (config*) malloc(sizeof(config));
    int tentacle_length = 3;
    int tentacle_count = 4;

    int cam_idx = 1;
    int light_idx = 2;
    int floor_idx = 3;
    int spider_joint_idx = 4;
    int spider_body_idx = 5;
    int tentacle_idx_offset = 6;

    C->frame_count = tentacle_length * tentacle_count * 2;  // Ball + Joint
    C->frame_count += 6;  // Root, Camera, Light, Floor, Spider Joint + Body
    C->frames = (frame**) malloc(sizeof(frame*) * C->frame_count);
    
    C->lights_count = 1;
    C->lights = (int*) malloc(sizeof(int) * C->lights_count);

    //---- Root Frame ----//
    frame* root = frame_init(origin_pos, origin_rot);
    root->pos_rel = NULL;
    root->rot_rel = NULL;

    root->children_count = 4;  // Camera, Light, Floor, Spider Joint
    root->children = (int*) malloc(sizeof(int) * root->children_count);
    root->children[0] = cam_idx;
    root->children[1] = light_idx;
    root->children[2] = floor_idx;
    root->children[3] = spider_joint_idx;
    
    root->data = NULL;

    root->parent = -1;
    root->type = 0;

    C->frames[0] = root;

    //---- Spider Free Joint ----//
    float spider_pos[] = {0.f, 0.f, 1.f};
    frame* spider_joint = frame_init(spider_pos, origin_rot);
    spider_joint->pos_rel = tensor_sub_give(spider_joint->pos, root->pos);
    spider_joint->rot_rel = tensor_copy(root->rot);

    spider_joint->children_count = 1;
    spider_joint->children = (int*) malloc(sizeof(int));
    spider_joint->children[0] = spider_body_idx;
    
    joint_t* joint_data = (joint_t*) malloc(sizeof(joint_t));
    joint_data->type = 3;
    joint_data->has_limits = 0;
    joint_data->q_id = 0;
    spider_joint->data = (void*) joint_data;

    spider_joint->parent = 0;
    spider_joint->type = 4;

    C->frames[spider_joint_idx] = spider_joint;

    //---- Spider Body ----//
    frame* spider_body = frame_init(spider_pos, origin_rot);
    spider_body->pos_rel = tensor_sub_give(spider_body->pos, spider_joint->pos);
    spider_body->rot_rel = tensor_copy(root->rot);

    spider_body->children_count = tentacle_count;
    spider_body->children = (int*) malloc(sizeof(int) * spider_body->children_count);
    
    geom* ball_geom = ball_geom_give(0.2f);
    ball_geom->tex->color[0] = 1.f;
    ball_geom->tex->color[1] = 0.f;
    ball_geom->tex->color[2] = 0.f;
    spider_body->data = (void*) ball_geom;

    spider_body->parent = spider_joint_idx;
    spider_body->type = 1;

    C->frames[spider_body_idx] = spider_body;

    //---- Floor Frame ----//
    // float floor_radius = 6371000.f;  // Earth
    // float floor_radius = 1737500.f;  // Moon
    float floor_radius = 1000.f;
    float floor_pos[] = {0.f, 0.f, -floor_radius};
    frame* floor = frame_init(floor_pos, origin_rot);

    geom* floor_geom = ball_geom_give(floor_radius);
    floor_geom->tex->color[0] = 1.f;
    floor_geom->tex->color[1] = 1.f;
    floor_geom->tex->color[2] = 0.f;
    floor->data = (void*) floor_geom;

    floor->parent = 0;
    floor->pos_rel = tensor_sub_give(floor->pos, root->pos);
    floor->rot_rel = tensor_copy(root->rot);
    floor->type = 1;

    C->frames[floor_idx] = floor;

    //---- Camera Frame ----//
    float cam_pos[] = {0.f, -2.f, 1.f};
    frame* cam = frame_init(cam_pos, origin_rot);

    camera_t* cam_data = (camera_t*) malloc(sizeof(camera_t));
    cam_data->fx = 0.1f;
    cam_data->fy = 0.1f;
    cam->data = (void*) cam_data;

    cam->parent = 0;
    cam->pos_rel = tensor_sub_give(cam->pos, root->pos);
    cam->rot_rel = tensor_copy(root->rot);
    cam->type = 2;

    C->frames[cam_idx] = cam;

    //---- Light Frame ----//
    float light_pos[] = {5.f, -5.f, 5.f};
    frame* light = frame_init(light_pos, origin_rot);

    light_t* light_data = (light_t*) malloc(sizeof(light_t));
    light_data->intensity = 1.0f;
    light->data = (void*) light_data;

    light->parent = 0;
    light->pos_rel = tensor_sub_give(light->pos, root->pos);
    light->rot_rel = tensor_copy(root->rot);
    light->type = 3;

    C->frames[light_idx] = light;
    C->lights[0] = light_idx;

    //---- Ball Frames ----//
    int q_id = 7;
    for (int i = 0; i < tentacle_count; i++)
    {
        spider_body->children[i] = (i * tentacle_length * 2) + tentacle_idx_offset;
        int prev_piece = -1;
        for (int j = 0; j < tentacle_length; j++)
        {
            float pos_values[] = {0.f, 0.f, 1.f};
            int j1 = j+1;
            if (i == 0) pos_values[0] =  0.2f * (float) j1 + 0.1f;
            if (i == 1) pos_values[1] =  0.2f * (float) j1 + 0.1f;
            if (i == 2) pos_values[0] = -0.2f * (float) j1 - 0.1f;
            if (i == 3) pos_values[1] = -0.2f * (float) j1 - 0.1f;
            
            int joint_idx = (i * tentacle_length + j) * 2 + tentacle_idx_offset;
            int parent_idx = prev_piece == -1 ? spider_body_idx : prev_piece;
            add_tentacle_piece_to_config(C, pos_values, joint_idx, parent_idx, j == tentacle_length-1, q_id, j, i);
            q_id++;
            prev_piece = joint_idx + 1;
        }
    }

    // Fill in joint array and qs
    int q_dim = 0;
    int q_vel_dim = 0;
    C->joints = NULL;
    for (int i = 0; i < C->frame_count; i++)
    {
        if (C->frames[i]->type == 4)
        {
            C->joints_count++;
            joint_t* joint_data = C->frames[i]->data;
            int joint_type = joint_data->type;
            if (joint_type == 0 || joint_type == 1 || joint_type == 2)
            {
                q_dim++;
                q_vel_dim++;
            }
            else if (joint_type == 3)
            {
                q_dim += 7;
                q_vel_dim += 6;
            }
            #ifdef DEBUG
            else
            {
                printf("Invalid joint type!\n");
                exit(EXIT_FAILURE);
            }
            #endif
        }
    }
    int index = 0;
    if (C->joints_count)
    {
        C->joints = (int*) malloc(sizeof(int)*C->joints_count);
        for (int i = 0; i < C->frame_count; i++)
        {
            if (C->frames[i]->type == 4)
            {
                C->joints[index] = i;
                index++;
            }
        }
    }
    
    int q_shape[] = {q_dim, 1};
    C->q = new_tensor(q_shape, 2, NULL);
    C->q_min = new_tensor(q_shape, 2, NULL);
    C->q_max = new_tensor(q_shape, 2, NULL);
    q_shape[0] = q_vel_dim;
    C->q_vel = new_tensor(q_shape, 2, NULL);
    tensor_fill(C->q_min, -1.f);
    tensor_fill(C->q_max, 1.f);

    return C;
}
