#include <stdlib.h>
#include <stdio.h>

#include "configuration.h"


int config_colliding(config* C)
{
    // TODO: KD_Tree
    for (int i = 0; i < C->frame_count; i++)
    {
        frame* a = C->frames[i];
        for (int j = 0; j < C->frame_count; j++)
        {
            frame* b = C->frames[j];
        }
    }
    return 0;
}

void config_free(config* C)
{
    for (int i = 0; i < C->frame_count; i++)
    {
        frame_free(C->frames[i]);
    }
    tensor_free(C->q);
    tensor_free(C->q_max);
    tensor_free(C->q_min);
    free(C);
}
