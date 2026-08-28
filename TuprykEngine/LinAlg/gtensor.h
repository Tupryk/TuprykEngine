#ifndef GTENSOR
#define GTENSOR

#include "tensor.h"
#include "../Algos/lists.h"

typedef struct
{


} grad_elem_t;

tensor_t* new_gtensor_vector(int dim, float* values);
tensor_t* gtensor_backward(tensor_t* gt);
void gtensor_scalar_add(tensor_t* a, float b, tensor_t* out);
void gtensor_mult(tensor_t* a, tensor_t* b, tensor_t* out);
void gtensor_xTx(tensor_t* x, tensor_t* out);

#endif
