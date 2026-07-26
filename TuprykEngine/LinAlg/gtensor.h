#ifndef GTENSOR
#define GTENSOR

#include "tensor.h"

typedef struct
{
    tensor_t* t;
    stack_t* grad_chain;
} gtensor_t;

void gtensor_add(gtensor_t* a, gtensor_t* b, gtensor_t* out);

#endif
