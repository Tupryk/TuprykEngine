#ifndef QUATERNIONS
#define QUATERNIONS

#include "../LinAlg/tensor.h"

void quaternion_from_euler(float x, float y, float z, float* out);
void quaternion_x(float phi, float* out);
void quaternion_y(float phi, float* out);
void quaternion_z(float phi, float* out);
void quaternion_to_matrix(float* q, tensor* out);
tensor* quaternion_to_matrix_give(float* q);
void quaternion_conjugate(float* q, float* out);
void quaternion_product(float* a, float* b, float* out);
void quaternion_rotate_point(float* q, float* p, float* out);
void quaternion_normalize(float* q);

#endif
