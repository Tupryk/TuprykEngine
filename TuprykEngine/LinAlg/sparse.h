#ifndef SPARSE
#define SPARSE

#define SPARSE_PRECISION 1e-4

#include "tensor.h"


struct sparse
{
    int* shape;
    int shape_dim;
    int volume;
    float* values;
    int* indices;
    int value_count;
};

struct sparse* new_sparse(int* shape, int shape_dim);
struct sparse* sparse_from_tensor(struct tensor* t);
struct sparse* sparse_from_func(int* shape, int shape_dim, float (*func)(int));
struct tensor* tensor_from_sparse(struct sparse* st);
void sparse_free(struct sparse* st);
void sparse_insert(struct sparse* st, float value, int index);

#endif
