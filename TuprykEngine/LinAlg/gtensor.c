#include <stdio.h>
#include <stdlib.h>

#include "gtensor.h"


gtensor_t* new_gtensor_vector(int dim, float* values)
{
    gtensor_t* gt = (gtensor_t*) malloc(sizeof(gtensor_t));
    gt->t = new_tensor_vector(dim, values);
    gt->grad_chain = stack_init();
    return gt;
}

tensor_t* gtensor_backward(gtensor_t* gt)
{
    return tensor_copy(gt->t);
}

void gtensor_scalar_add(tensor_t* a, float b, gtensor_t* out)
{
    tensor_scalar_add(a, b, out->t);
}

void gtensor_scalar_pow(tensor_t* a, float b, gtensor_t* out)
{
    tensor_scalar_pow(a, b, out->t);
}

void gtensor_free(gtensor_t* gt)
{
    tensor_free(gt->t);
    while (gt->grad_chain->next != NULL)
    {
        grad_elem_t* ge = stack_pop(gt->grad_chain);
        // TODO: free ge
    }
    free(gt->grad_chain);
    free(gt);
}
