#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../LinAlg/tensor.h"

typedef struct
{
    int frame_count;
    frame** frames;  // Frame at index 0 is always root!!!
    
    tensor* q;
    tensor* q_max;
    tensor* q_min;
} config;

int config_colliding(config* C);
void config_free(config* C);

#endif
