#ifndef TENSOR
#define TENSOR

#include "../global.h"

typedef struct
{
    int* shape;
    int shape_dim;
    float* values;
    int volume;
    int is_slice;
    struct tensor_t* grad;
    
    // This could speed things up in a lot of cases (eg. Inverting). Value needs to be updated in certain cases.
    int type;  // -1: None, 0: diagonal, 1: orthogonal, 2: PSD // TODO: Decide this. Maybe each bit can encode a certain thing? Like positive, etc.
} tensor_t;

// Switch to always using tensor pointers (faster) ei. tensor new_tensor(...); -> tensor_t* new_tensor(...);
tensor_t* new_tensor(int* shape, int shape_dim, float* values);
tensor_t* new_tensor_slice(int* shape, int shape_dim, float* values);
tensor_t* new_tensor_diagonal(int dim, float* values);
tensor_t* new_tensor_diagonal_uniform(int dim, float value);
tensor_t* new_tensor_vector(int dim, float* values);
tensor_t* new_tensor_matrix(int cols, int rows, float* values);
tensor_t* tensor_copy(tensor_t* t);
tensor_t* tensor_copy_shape(tensor_t* t);
void tensor_fill(tensor_t* t, float value);
void tensor_fill_uniform(tensor_t* t, float min, float max);
void tensor_fill_gauss(tensor_t* t, float mean, float std);
void tensor_transfer_values(tensor_t* to, tensor_t* from);
void tensor_set_values(tensor_t* t, float* values);
void tensor_clip(tensor_t* t, float min, float max);
void tensor_free(tensor_t* t);
int get_tensor_volume(tensor_t* t);
int get_tensor_value_index(tensor_t* t, int* indices);
double get_tensor_value(tensor_t* t, int* indices);
void get_identity(int shape_dim, int shape);
void fill_random(tensor_t* t);
void fill_const(tensor_t* t, double num);
int tensors_can_be_mult(tensor_t* a, tensor_t* b);
void tensor_mult(tensor_t* a, tensor_t* b, tensor_t* out);
tensor_t* tensor_mult_give(tensor_t* a, tensor_t* b);
void tensor_add(tensor_t* a, tensor_t* b, tensor_t* out);
void tensor_sub(tensor_t* a, tensor_t* b, tensor_t* out);
void tensor_scalar_add(tensor_t* a, float b, tensor_t* out);
void tensor_scalar_sub(tensor_t* a, float b, tensor_t* out);
void tensor_scalar_pow(tensor_t* a, float b, tensor_t* out);
tensor_t* tensor_add_give(tensor_t* a, tensor_t* b);
tensor_t* tensor_sub_give(tensor_t* a, tensor_t* b);
void tensor_loop(tensor_t* t, void (*func)(tensor_t*, int*));
void tensor_identity(tensor_t* a);
void tensor_transpose(tensor_t* t);
int tensors_equal_shape(tensor_t* a, tensor_t* b);
int tensors_equal(tensor_t* a, tensor_t* b);
void tensor_scalar_mult(tensor_t* a, float b, tensor_t* out);
tensor_t* tensor_scalar_mult_give(tensor_t* a, float b);
void tensor_scalar_div(tensor_t* a, float b, tensor_t* out);
tensor_t* tensor_scalar_div_give(tensor_t* a, float b);
int tensor_is_square(tensor_t* t);
void tensor_reshape(tensor_t* t, int* shape, int shape_dim);
tensor_t* tensor_append(tensor_t* a, tensor_t* b, int axis);
int tensor_lu_decomp(tensor_t* A, tensor_t* P, tensor_t* L, tensor_t* U);
float tensor_determinant(tensor_t* A);
void tensor_inverse(tensor_t* A, tensor_t* A_inv);
tensor_t* tensor_inverse_give(tensor_t* A);
void tensor_flatten(tensor_t* t);
float tensor_max(tensor_t* t);
float tensor_xTAx(tensor_t* A, tensor_t* x);
void tensor_xTx(tensor_t* x, tensor_t* out);
float tensor_min(tensor_t* t);
float tensor_trace(tensor_t* t);
tensor_t* tensor_slice(tensor_t* t, int idx);
void tensor_conv2d(tensor_t* kernel, tensor_t* image, tensor_t* out);
void vector_cross(tensor_t* a, tensor_t* b, tensor_t* out);
tensor_t* vector_cross_give(tensor_t* a, tensor_t* b);
float vector_diff_norm(tensor_t* a, tensor_t* b);
float vector_dot(tensor_t* a, tensor_t* b);
float vector_norm(tensor_t* t);
float vector_squared_norm(tensor_t* t);
void vector_normalize(tensor_t* t);
void vector_reflect(tensor_t* v, tensor_t* n, tensor_t* out);
tensor_t* vector_reflect_give(tensor_t* v, tensor_t* n);

#endif
