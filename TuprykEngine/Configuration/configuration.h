#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../LinAlg/tensor.h"

struct config
{
    int frame_count;
    struct frame* frames;
    
    tensor* q;
    tensor* q_max;
    tensor* q_min;
};

int config_colliding(struct config* C);
void config_free(struct config* C);

#endif
