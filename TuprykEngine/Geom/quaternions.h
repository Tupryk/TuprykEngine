#ifndef QUATERNIONS
#define QUATERNIONS

#include "../LinAlg/tensor.h"

void quaternion_to_matrix(tensor* q, tensor* out);
tensor* quaternion_to_matrix_give(tensor* q);

#endif
