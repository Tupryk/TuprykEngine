#ifndef TENSOR
#define TENSOR

#include "../global.h"

struct sparse_tensor
{
    int* shape;
    int shape_dim;
    TYPE* values;
    int** indices;
};

struct tensor
{
    int* shape;
    int shape_dim;
    TYPE* values;
    int volume;
    // This could speed things up in a lot of cases (eg. Inverting). Value needs to be updated in certain cases.
    int type;  // -1: None, 0: diagonal, 1: orthogonal, 2: PSD // TODO: Decide this. Maybe each bit can encode a certain thing? Like positive, etc.
};

// Switch to always using tensor pointers (faster) ei. struct tensor new_tensor(...); -> struct tensor* new_tensor(...);
struct tensor* new_tensor(int* shape, int shape_dim, TYPE* values);
struct tensor* tensor_copy(struct tensor* t);
void free_tensor(struct tensor* t);
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
int tensors_equal(struct tensor* a, struct tensor* b);
void tensor_scalar_mult(struct tensor* a, TYPE b, struct tensor* out);
int tensor_is_square(struct tensor* t);
void tensor_reshape(struct tensor* t, int* shape, int shape_dim);
struct tensor* tensor_append(struct tensor* a, struct tensor* b, int axis);

// void singular_value_decomposition(struct tensor* target, struct tensor* U, struct tensor* eta, struct tensor* V_t);
// struct tensor* pseudo_inverse(struct tensor* target);
// void pseudo_inverse(struct tensor* target, struct tensor* out);
// Eigenvalues / Eigenvectors

// TODO:
// If your goal is to compute 
// A−1b
// A
// −1
// b, never compute 
// A−1
// A
// −1
// .

// Instead:

// Factor 
// A=LU
// A=LU

// Solve 
// LUx=b
// LUx=b

#endif
