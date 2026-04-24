#ifndef LINALG_VIS
#define LINALG_VIS

#include "../../LinAlg/tensor.h"
#include "../../LinAlg/sparse.h"

void print_tensor(tensor* t);
void print_tensor_verbose(tensor* t, int verbose);
void print_tensor_values(tensor* t);
void print_tensor_flat(tensor* t);
void print_shape(int* shape, int shape_dim);
void print_sparse(sparse* st);

#endif
