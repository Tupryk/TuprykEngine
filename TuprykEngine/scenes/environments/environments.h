#ifndef ENVIRONMENT
#define ENVIRONMENT

#include "../LinAlg/tensor.h"


typedef struct
{
    int done;
    float reward;
    tensor* state;
} env_return_t;

typedef struct
{
    int time;
    tensor* state;
} env_t;

env_return_t* env_step(env_t* e);

#endif
