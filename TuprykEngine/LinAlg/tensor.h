#ifndef TENSOR
#define TENSOR

#include "../global.h"

struct tensor
{
    int* shape;
    int shape_dim;
    float* values;
    int volume;
    // This could speed things up in a lot of cases (eg. Inverting). Value needs to be updated in certain cases.
    int type;  // -1: None, 0: diagonal, 1: orthogonal, 2: PSD // TODO: Decide this. Maybe each bit can encode a certain thing? Like positive, etc.
};

// Switch to always using tensor pointers (faster) ei. struct tensor new_tensor(...); -> struct tensor* new_tensor(...);
struct tensor* new_tensor(int* shape, int shape_dim, float* values);
struct tensor* tensor_copy(struct tensor* t);
struct tensor* tensor_copy_shape(struct tensor* t);
void tensor_transfer_values(struct tensor* to, struct tensor* from);
void tensor_free(struct tensor* t);
int get_tensor_volume(struct tensor* t);
int get_tensor_value_index(struct tensor* t, int* indices);
double get_tensor_value(struct tensor* t, int* indices);
void get_identity(int shape_dim, int shape);
void fill_random(struct tensor* t);
void fill_const(struct tensor* t, double num);
int tensors_can_be_mult(struct tensor* a, struct tensor* b);
void tensor_mult(struct tensor* a, struct tensor* b, struct tensor* out);
struct tensor* tensor_mult_give(struct tensor* a, struct tensor* b);
void tensor_add(struct tensor* a, struct tensor* b, struct tensor* out);
void tensor_sub(struct tensor* a, struct tensor* b, struct tensor* out);
void tensor_loop(struct tensor* t, void (*func)(struct tensor*, int*));
void tensor_identity(struct tensor* a);
void tensor_transpose(struct tensor* t);
int tensors_equal_shape(struct tensor* a, struct tensor* b);
int tensors_equal(struct tensor* a, struct tensor* b);
void tensor_scalar_mult(struct tensor* a, float b, struct tensor* out);
struct tensor* tensor_scalar_mult_give(struct tensor* a, float b);
int tensor_is_square(struct tensor* t);
void tensor_reshape(struct tensor* t, int* shape, int shape_dim);
struct tensor* tensor_append(struct tensor* a, struct tensor* b, int axis);
int tensor_lu_decomp(struct tensor* A, struct tensor* P, struct tensor* L, struct tensor* U);
float tensor_determinant(struct tensor* A);
void tensor_inverse(struct tensor* A, struct tensor* A_inv);
struct tensor* tensor_inverse_give(struct tensor* A);
float tensor_vec_magnitude(struct tensor* t);
void tensor_flatten(struct tensor* t);
float tensor_max(struct tensor* t);
float tensor_min(struct tensor* t);


// void singular_value_decomposition(struct tensor* target, struct tensor* U, struct tensor* eta, struct tensor* V_t);
// struct tensor* pseudo_inverse(struct tensor* target);
// void pseudo_inverse(struct tensor* target, struct tensor* out);
// Eigenvalues / Eigenvectors

#endif
