#ifndef GTENSOR
#define GTENSOR

#include "tensor.h"
#include "../Algos/lists.h"

typedef struct
{

} grad_elem_t;

typedef struct
{
    tensor_t* t;
    pstack_t* grad_chain;
} gtensor_t;

gtensor_t* new_gtensor_vector(int dim, float* values);
tensor_t* gtensor_backward(gtensor_t* gt);
void gtensor_scalar_add(tensor_t* a, float b, gtensor_t* out);
void gtensor_scalar_pow(tensor_t* a, float b, gtensor_t* out);
void gtensor_free(gtensor_t* gt);

#endif
