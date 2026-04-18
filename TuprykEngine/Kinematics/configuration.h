#ifndef CONFIGURATION
#define CONFIGURATION

#include "frame.h"
#include "../LinAlg/tensor.h"

typedef struct
{
    int frame_count;
    frame** frames;  // Frame at index 0 is always root!!!

    int joints_count;
    int* joints;

    int lights_count;
    int* lights;
    
    tensor* q;
    tensor* q_max;
    tensor* q_min;
} config;

int config_colliding(config* C);
void config_set_q(config* C, float* q);
void config_update_q(config* C);
void config_free(config* C);

#endif
