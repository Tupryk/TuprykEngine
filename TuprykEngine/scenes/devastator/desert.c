#include <stdio.h>
#include <stdlib.h>

#include "desert.h"
#include "../../Stochastic/sample.h"


geom* ball_geom_give(float r, float g, float b, float radius)
{
    geom* ball_geom = (geom*) malloc(sizeof(geom));
    
    ball_geom->type = 0;
    
    ball_geom->mesh = (void*) malloc(sizeof(float));
    *(float*)ball_geom->mesh = radius;

    ball_geom->tex = (texture*) malloc(sizeof(texture));

    ball_geom->tex->color[0] = r;
    ball_geom->tex->color[1] = g;
    ball_geom->tex->color[2] = b;

    ball_geom->tex->ambient  = 0.5f;
    ball_geom->tex->diffuse  = 1.0f;
    ball_geom->tex->specular = 1.0f;
    ball_geom->tex->shininess = 100.f;

    return ball_geom;
}

config* init_devastator_config()
{
    config* C = (config*) malloc(sizeof(config));
    int tentacle_length = 5;
    int tentacle_count = 4;

    C->frame_count = 1 + tentacle_length * tentacle_count;
    C->frame_count += 2;  // Camera! Light!
    C->frames = (frame**) malloc(sizeof(frame*) * C->frame_count);
    
    C->lights_count = 1;
    C->lights = (int*) malloc(sizeof(int) * C->lights_count);

    //---- Root Frame ----//
    float root_pos[] = {0.f, 0.f, 0.f};
    float root_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* root = frame_init(root_pos, root_rot);

    root->children_count = tentacle_count;
    root->children_count += 2;  // Camera! Light!
    root->children = (int*) malloc(sizeof(int) * root->children_count);

    root->data = (void*) ball_geom_give(1.f, 0.f, 0.f, 0.2f);

    root->parent = -1;
    root->type = 1;

    C->frames[0] = root;

    //---- Ball Frames ----//
    for (int i = 0; i < tentacle_count; i++)
    {
        root->children[i] = i * tentacle_length + 1;
        int prev_piece = -1;
        for (int j = 0; j < tentacle_length; j++)
        {
            float pos_values[] = {0.f, 0.f, 0.f};
            int j1 = j+1;
            if (i == 0) pos_values[0] =  0.2f * (float) j1 + 0.1f;
            if (i == 1) pos_values[1] =  0.2f * (float) j1 + 0.1f;
            if (i == 2) pos_values[0] = -0.2f * (float) j1 + 0.1f;
            if (i == 3) pos_values[1] = -0.2f * (float) j1 + 0.1f;
            
            frame* ball = frame_init(pos_values, root_rot);

            if (prev_piece == -1) ball->parent = 0;
            else ball->parent = prev_piece;

            if (j == tentacle_length-1)
            {
                ball->children_count = 0;
                ball->children = NULL;
            }
            else
            {
                ball->children_count = 1;
                ball->children = (int*) malloc(sizeof(int));
                ball->children[0] = 2 + i * tentacle_length + j;
            }

            ball->data = (void*) ball_geom_give(
                rand_uni(0.f, 1.f),
                rand_uni(0.f, 1.f),
                rand_uni(0.f, 1.f),
                0.1f
            );

            ball->type = 1;

            prev_piece = 1 + i * tentacle_length + j;
            C->frames[prev_piece] = ball;
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
    cam->type = 2;

    float light_pos[] = {0.f, 0.f, 10.f};
    frame* light = frame_init(light_pos, root_rot);

    light_t* light_data = (light_t*) malloc(sizeof(light_t));
    light_data->intensity = 1.0f;
    light->data = (void*) light_data;

    light->parent = 0;
    light->type = 3;

    int cam_idx = C->frame_count-2;
    int light_idx = C->frame_count-1;
    root->children[root->children_count-2] = cam_idx;
    root->children[root->children_count-1] = light_idx;
    C->frames[cam_idx] = cam;
    C->frames[light_idx] = light;
    C->lights[0] = light_idx;
    
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

    root->data = (void*) ball_geom_give(1.f, 0.f, 0.f, 0.2f);

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
