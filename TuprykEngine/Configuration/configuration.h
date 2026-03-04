#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../LinAlg/tensor.h"

struct config
{
    int frame_count;
    struct frame* frames;
    
    struct tensor* q;
    struct tensor* q_max;
    struct tensor* q_min;
};

int config_colliding(struct config* C);
void config_free(struct config* C);

#endif
