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
    joint_data->q_ids = (int*) malloc(sizeof(int));
    joint_data->q_ids[0] = q_id;
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
    config* C = (config*) malloc(sizeof(config));
    int tentacle_length = 3;
    int tentacle_count = 4;

    C->frame_count = 1 + tentacle_length * tentacle_count * 2;
    C->frame_count += 2;  // Camera! Light!
    C->frame_count += 1;  // Floor!
    C->frames = (frame**) malloc(sizeof(frame*) * C->frame_count);
    
    C->lights_count = 1;
    C->lights = (int*) malloc(sizeof(int) * C->lights_count);

    //---- Root Frame ----//
    float root_pos[] = {0.f, 0.f, 0.f};
    float root_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* root = frame_init(root_pos, root_rot);
    root->pos_rel = NULL;
    root->rot_rel = NULL;

    root->children_count = tentacle_count;
    root->children_count += 2;  // Camera! Light!
    root->children_count += 1;  // Floor!
    root->children = (int*) malloc(sizeof(int) * root->children_count);
    root->children[root->children_count-1] = C->frame_count-1;  // Floor!
    
    geom* ball_geom = ball_geom_give(0.2f);
    ball_geom->tex->color[0] = 1.f;
    ball_geom->tex->color[1] = 0.f;
    ball_geom->tex->color[2] = 0.f;
    root->data = (void*) ball_geom;

    root->parent = -1;
    root->type = 1;

    C->frames[0] = root;

    //---- Floor Frame ----//
    // float floor_radius = 6371000.f;  // Earth
    // float floor_radius = 1737500.f;  // Moon
    float floor_radius = 1000.f;
    float floor_pos[] = {0.f, 0.f, -floor_radius - 1.f};
    frame* floor = frame_init(floor_pos, root_rot);

    floor->children_count = 0;
    floor->children = NULL;
    
    geom* floor_geom = ball_geom_give(floor_radius);
    floor_geom->tex->color[0] = 1.f;
    floor_geom->tex->color[1] = 1.f;
    floor_geom->tex->color[2] = 0.f;
    floor->data = (void*) floor_geom;

    floor->parent = 0;
    floor->pos_rel = tensor_sub_give(floor->pos, root->pos);
    floor->rot_rel = tensor_copy(root->rot);
    floor->type = 1;

    C->frames[C->frame_count-1] = floor;

    //---- Ball Frames ----//
    int q_id = 0;
    for (int i = 0; i < tentacle_count; i++)
    {
        root->children[i] = (i * tentacle_length * 2) + 1;
        int prev_piece = -1;
        for (int j = 0; j < tentacle_length; j++)
        {
            float pos_values[] = {0.f, 0.f, 0.f};
            int j1 = j+1;
            if (i == 0) pos_values[0] =  0.2f * (float) j1 + 0.1f;
            if (i == 1) pos_values[1] =  0.2f * (float) j1 + 0.1f;
            if (i == 2) pos_values[0] = -0.2f * (float) j1 - 0.1f;
            if (i == 3) pos_values[1] = -0.2f * (float) j1 - 0.1f;
            
            int joint_idx = (i * tentacle_length + j) * 2 + 1;
            int parent_idx = prev_piece == -1 ? 0 : prev_piece;
            add_tentacle_piece_to_config(C, pos_values, joint_idx, parent_idx, j == tentacle_length-1, q_id, j, i);
            q_id++;
            prev_piece = joint_idx + 1;
        }
    }

    // Camera
    float cam_pos[] = {0.f, -2.f, 0.f};
    float cam_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* cam = frame_init(cam_pos, cam_rot);

    camera_t* cam_data = (camera_t*) malloc(sizeof(camera_t));
    cam_data->fx = 0.1f;
    cam_data->fy = 0.1f;
    cam->data = (void*) cam_data;

    cam->parent = 0;
    cam->pos_rel = tensor_sub_give(cam->pos, root->pos);
    cam->rot_rel = tensor_copy(root->rot);
    cam->type = 2;

    float light_pos[] = {5.f, -5.f, 5.f};
    frame* light = frame_init(light_pos, root_rot);

    light_t* light_data = (light_t*) malloc(sizeof(light_t));
    light_data->intensity = 1.0f;
    light->data = (void*) light_data;

    light->parent = 0;
    light->pos_rel = tensor_sub_give(light->pos, root->pos);
    light->rot_rel = tensor_copy(root->rot);
    light->type = 3;

    int cam_idx = C->frame_count-3;
    int light_idx = C->frame_count-2;
    root->children[root->children_count-3] = cam_idx;
    root->children[root->children_count-2] = light_idx;
    C->frames[cam_idx] = cam;
    C->frames[light_idx] = light;
    C->lights[0] = light_idx;

    // Fill in joint array
    C->joints = NULL;
    for (int i = 0; i < C->frame_count; i++)
    {
        if (C->frames[i]->type == 4)
        {
            C->joints_count++;
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
    
    int q_shape[] = {tentacle_count * tentacle_length, 1};
    C->q = new_tensor(q_shape, 2, NULL);
    C->q_min = new_tensor(q_shape, 2, NULL);
    tensor_fill(C->q_min, -1.f);
    C->q_max = new_tensor(q_shape, 2, NULL);
    tensor_fill(C->q_max, 1.f);

    return C;
}

config* init_just_ball_config()
{
    config* C = (config*) malloc(sizeof(config));

    C->frame_count = 3;
    C->frames = (frame**) malloc(sizeof(frame*) * C->frame_count);
    
    C->lights_count = 1;
    C->lights = (int*) malloc(sizeof(int) * C->lights_count);

    //---- Root Frame ----//
    float root_pos[] = {0.f, 0.f, 0.f};
    float root_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* root = frame_init(root_pos, root_rot);

    root->children_count = 2;
    root->children = (int*) malloc(sizeof(int) * root->children_count);

    root->data = (void*) ball_geom_give(0.2f);

    root->parent = -1;
    root->type = 1;

    C->frames[0] = root;

    // Camera
    float cam_pos[] = {0.f, -2.f, 0.f};
    float cam_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* cam = frame_init(cam_pos, cam_rot);

    camera_t* cam_data = (camera_t*) malloc(sizeof(camera_t));
    cam_data->fx = 0.1f;
    cam_data->fy = 0.1f;
    cam->data = (void*) cam_data;

    cam->parent = 0;
    cam->type = 2;

    float light_pos[] = {0.f, 0.f, 10.f};
    frame* light = frame_init(light_pos, root_rot);

    light_t* light_data = (light_t*) malloc(sizeof(light_t));
    light_data->intensity = 1.0f;
    light->data = (void*) light_data;

    light->parent = 0;
    light->type = 3;

    root->children[0] = 1;
    root->children[1] = 2;
    C->frames[1] = cam;
    C->frames[2] = light;
    C->lights[0] = 2;
    
    C->q = NULL;
    C->q_max = NULL;
    C->q_min = NULL;

    return C;
}
