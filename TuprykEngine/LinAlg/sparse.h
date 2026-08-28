#ifndef SPARSE
#define SPARSE

#define SPARSE_PRECISION 1e-4

#include "tensor.h"


typedef struct
{
    int* shape;
    int shape_dim;
    int volume;
    float* values;
    int* indices;
    int value_count;
} sparse_t;

sparse_t* new_sparse(int* shape, int shape_dim);
sparse_t* new_sparse_vector(int dim);
sparse_t* new_sparse_matrix(int cols, int rows);
sparse_t* sparse_copy(sparse_t* s);
sparse_t* sparse_from_tensor(tensor_t* t);
sparse_t* sparse_from_func(int* shape, int shape_dim, float (*func)(int));
tensor_t* tensor_from_sparse(sparse_t* st);
void sparse_free(sparse_t* st);
void sparse_insert(sparse_t* st, float value, int index);
sparse_t* sparse_transpose(sparse_t* st);
float sparse_get_density(sparse_t* st);

#endif
