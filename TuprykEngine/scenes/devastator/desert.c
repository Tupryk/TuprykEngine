#include <stdio.h>
#include <stdlib.h>

#include "desert.h"


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

    root->data = (void*) malloc(sizeof(float));
    *(float*)root->data = 0.2f;
    root->mass = 0.2f;

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

            ball->data = (void*) malloc(sizeof(float));
            *(float*)ball->data = 0.1f;
            root->mass = 0.1f;

            ball->type = 1;

            prev_piece = 1 + i * tentacle_length + j;
            C->frames[prev_piece] = ball;
        }
    }

    // Camera
    float cam_pos[] = {0.f, -2.f, 0.f};
    float cam_rot[] = {1.f, 0.f, 0.f, 0.f};
    frame* cam = frame_init(cam_pos, cam_rot);

    cam->data = (void*) malloc(sizeof(float));
    *(float*)cam->data = 0.1f;

    cam->parent = 0;
    cam->type = 2;

    float light_pos[] = {0.f, 0.f, 1.f};
    frame* light = frame_init(light_pos, root_rot);

    light->parent = 0;
    light->type = 3;

    int cam_idx = C->frame_count-2;
    int light_idx = C->frame_count-1;
    root->children[root->children_count-2] = cam_idx;
    root->children[root->children_count-1] = light_idx;
    C->frames[cam_idx] = cam;
    C->frames[light_idx] = light;
    C->lights[0] = light_idx;
    
    C->q = NULL;
    C->q_max = NULL;
    C->q_min = NULL;

    return C;
}
