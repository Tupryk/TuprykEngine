#include "gtensor.h"

void gtensor_add(gtensor_t* a, gtensor_t* b, gtensor_t* out)
{
    tensor_add();
    a->grad_chain++;
}
