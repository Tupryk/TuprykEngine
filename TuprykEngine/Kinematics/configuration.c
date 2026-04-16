#include <stdlib.h>
#include <stdio.h>

#include "configuration.h"


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
    free(C);
}
