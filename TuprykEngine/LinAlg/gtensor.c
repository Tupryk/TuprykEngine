#include <stdio.h>
#include <stdlib.h>

#include "gtensor.h"


void gtensor_backward(tensor_t* gt)
{
    return tensor_copy(gt->t);
}

void gtensor_scalar_add(tensor_t* a, float b, tensor_t* out)
{
    tensor_scalar_add(a, b, out->t);
}

void gtensor_mult(tensor_t* a, tensor_t* b, tensor_t* out)
{
    tensor_mult(a, b, out);
}

void gtensor_xTx(tensor_t* x, tensor_t* out)
{
    tensor_xTx(x, out);
}
