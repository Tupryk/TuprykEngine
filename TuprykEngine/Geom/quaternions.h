#ifndef QUATERNIONS
#define QUATERNIONS

#include "../LinAlg/tensor.h"

void quaternion_from_euler(float x, float y, float z, float* out);
void quaternion_to_euler(float* q, float* out);
void quaternion_x(float phi, float* out);
void quaternion_y(float phi, float* out);
void quaternion_z(float phi, float* out);
void quaternion_to_matrix(float* q, tensor_t* out);
tensor_t* quaternion_to_matrix_give(float* q);
void quaternion_conjugate(float* q, float* out);
void quaternion_diff(float* q1, float* q2, float* out);
void quaternion_product(float* a, float* b, float* out);
void quaternion_rotate_point(float* q, float* p, float* out);
void quaternion_normalize(float* q);
void quaternion_delta(float* q, float* delta, float dt, float* out);
void quaternion_rotate(float* q, float* v, float* out);
void quaternion_to_rotvec(float* q, float* out);

#endif
