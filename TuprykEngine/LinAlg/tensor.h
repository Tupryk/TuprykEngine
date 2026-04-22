#ifndef TENSOR
#define TENSOR

#include "../global.h"

typedef struct
{
    int* shape;
    int shape_dim;
    float* values;
    int volume;
    // This could speed things up in a lot of cases (eg. Inverting). Value needs to be updated in certain cases.
    int type;  // -1: None, 0: diagonal, 1: orthogonal, 2: PSD // TODO: Decide this. Maybe each bit can encode a certain thing? Like positive, etc.
} tensor;

// Switch to always using tensor pointers (faster) ei. tensor new_tensor(...); -> tensor* new_tensor(...);
tensor* new_tensor(int* shape, int shape_dim, float* values);
tensor* new_tensor_diagonal(int dim, float* values);
tensor* new_tensor_vector(int dim, float* values);
tensor* new_tensor_matrix(int cols, int rows, float* values);
tensor* tensor_copy(tensor* t);
tensor* tensor_copy_shape(tensor* t);
void tensor_fill(tensor* t, float value);
void tensor_fill_uniform(tensor* t, float min, float max);
void tensor_transfer_values(tensor* to, tensor* from);
void tensor_set_values(tensor* t, float* values);
void tensor_free(tensor* t);
int get_tensor_volume(tensor* t);
int get_tensor_value_index(tensor* t, int* indices);
double get_tensor_value(tensor* t, int* indices);
void get_identity(int shape_dim, int shape);
void fill_random(tensor* t);
void fill_const(tensor* t, double num);
int tensors_can_be_mult(tensor* a, tensor* b);
void tensor_mult(tensor* a, tensor* b, tensor* out);
tensor* tensor_mult_give(tensor* a, tensor* b);
void tensor_add(tensor* a, tensor* b, tensor* out);
void tensor_sub(tensor* a, tensor* b, tensor* out);
void tensor_scalar_add(tensor* a, float b, tensor* out);
void tensor_scalar_sub(tensor* a, float b, tensor* out);
tensor* tensor_add_give(tensor* a, tensor* b);
tensor* tensor_sub_give(tensor* a, tensor* b);
void tensor_loop(tensor* t, void (*func)(tensor*, int*));
void tensor_identity(tensor* a);
void tensor_transpose(tensor* t);
int tensors_equal_shape(tensor* a, tensor* b);
int tensors_equal(tensor* a, tensor* b);
void tensor_scalar_mult(tensor* a, float b, tensor* out);
tensor* tensor_scalar_mult_give(tensor* a, float b);
int tensor_is_square(tensor* t);
void tensor_reshape(tensor* t, int* shape, int shape_dim);
tensor* tensor_append(tensor* a, tensor* b, int axis);
int tensor_lu_decomp(tensor* A, tensor* P, tensor* L, tensor* U);
float tensor_determinant(tensor* A);
void tensor_inverse(tensor* A, tensor* A_inv);
tensor* tensor_inverse_give(tensor* A);
void tensor_flatten(tensor* t);
float tensor_max(tensor* t);
float tensor_min(tensor* t);
float tensor_trace(tensor* t);
tensor* tensor_slice(tensor* t, int idx);
void vector_cross(tensor* a, tensor* b, tensor* out);
tensor* vector_cross_give(tensor* a, tensor* b);
float vector_dot(tensor* a, tensor* b);
float vector_norm(tensor* t);
float vector_squared_norm(tensor* t);
void vector_normalize(tensor* t);

// void singular_value_decomposition(tensor* target, tensor* U, tensor* eta, tensor* V_t);
// tensor* pseudo_inverse(tensor* target);
// void pseudo_inverse(tensor* target, tensor* out);
// Eigenvalues / Eigenvectors
// Diagonalisierung

#endif
