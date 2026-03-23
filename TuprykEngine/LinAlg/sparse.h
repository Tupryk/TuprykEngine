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
} sparse;

sparse* new_sparse(int* shape, int shape_dim);
sparse* sparse_copy(sparse* s);
sparse* sparse_from_tensor(tensor* t);
sparse* sparse_from_func(int* shape, int shape_dim, float (*func)(int));
tensor* tensor_from_sparse(sparse* st);
void sparse_free(sparse* st);
void sparse_insert(sparse* st, float value, int index);

#endif
