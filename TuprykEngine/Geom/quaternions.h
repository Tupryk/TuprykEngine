#ifndef QUATERNIONS
#define QUATERNIONS

#include "../LinAlg/tensor.h"

void quaternion_from_euler(float x, float y, float z, float* out);
void quaternion_to_matrix(float* q, tensor* out);
tensor* quaternion_to_matrix_give(float* q);

#endif
