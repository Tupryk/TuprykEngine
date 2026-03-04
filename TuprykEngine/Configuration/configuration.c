#include "configuration.h"

int config_colliding(struct config* C)
{
    for (int i = 0; i < C->frame_count; i++)
    {
        struct frame* a = &C->frames[i];
        for (int j = 0; j < C->frame_count; j++)
        {
            struct frame* b = &C->frames[j];

            struct tensor* diff = tensor_sub_give(a->pos, b->pos);
            float dist = tensor_norm(diff);
            if (dist <= a->r + b->r)
            {
                return 1;
            }
        }
    }
    return 0;
}

void config_free(struct config* C)
{
    tensor_free(C->q);
    tensor_free(C->q_max);
    tensor_free(C->q_min);
    free(C);
}
