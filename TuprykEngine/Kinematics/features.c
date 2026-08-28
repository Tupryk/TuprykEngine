#include <stdio.h>
#include <stdlib.h>
#include "features.h"


void p2p_vec(config_t* C, int parent_id, tensor_t* rel_pos, tensor_t* target, gtensor_t* out)
{
    #ifdef DEBUG
    if (out->t->shape_dim != 2 || out->t->shape[0] != 3 || out->t->shape[1] != 1)
    {
        printf("Invalid output tensor for p2p_vec config feature.\n");
        exit(EXIT_FAILURE);
    }
    if (parent_id >= C->frame_count)
    {
        printf("parent_id is greater than frame_coint for p2p_vec config feature.\n");
        exit(EXIT_FAILURE);
    }
    #endif

    out->t->values[0] = 1.f;
    out->t->values[1] = 2.f;
    out->t->values[2] = 3.f;
}
