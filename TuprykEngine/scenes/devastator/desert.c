#include <stdio.h>
#include <stdlib.h>

#include "desert.h"


config* init_devastator_config()
{
    config* C = (config*) malloc(sizeof(config));
    int tentacle_length = 5;
    int tentacle_count = 4;
    C->frame_count = 1 + tentacle_length * tentacle_count;

    C->frames = (frame**) malloc(sizeof(frame*) * C->frame_count);

    //---- Root Frame ----//
    frame* root = (frame*) malloc(sizeof(frame));

    int pos_shape[] = {3, 1};
    float pos_values[] = {0, 0, 0};
    root->pos = new_tensor(pos_shape, 2, pos_values);

    int rot_shape[] = {4, 1};
    float rot_values[] = {1, 0, 0, 0};
    root->rot = new_tensor(rot_shape, 2, rot_values);

    root->children_count = tentacle_count;
    root->children = (int*) malloc(sizeof(int) * root->children_count);

    root->data = (void*) malloc(sizeof(float));
    *(float*)root->data = 0.2f;

    root->type = 1;

    C->frames[0] = root;

    //---- Ball Frames ----//
    for (int i = 0; i < tentacle_count; i++)
    {
        root->children[i] = i+1;
        int prev_piece = -1;
        for (int j = 0; j < tentacle_length; j++)
        {
            frame* ball = (frame*) malloc(sizeof(frame));

            int pos_shape[] = {3, 1};
            float pos_values[] = {0, 0, 0};
            int j1 = j+1;
            if (i == 0) pos_values[0] =  0.15f * (float) j1;
            if (i == 1) pos_values[1] =  0.15f * (float) j1;
            if (i == 2) pos_values[0] = -0.15f * (float) j1;
            if (i == 3) pos_values[1] = -0.15f * (float) j1;
            ball->pos = new_tensor(pos_shape, 2, pos_values);

            int rot_shape[] = {4, 1};
            float rot_values[] = {1, 0, 0, 0};
            ball->rot = new_tensor(rot_shape, 2, rot_values);

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

            ball->type = 1;

            prev_piece = 1 + i * tentacle_length + j;
            C->frames[prev_piece] = ball;
        }
    }
    
    C->q = NULL;
    C->q_max = NULL;
    C->q_min = NULL;

    return C;
}
