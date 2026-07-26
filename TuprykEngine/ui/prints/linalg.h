#ifndef LINALG_VIS
#define LINALG_VIS

#include "../../LinAlg/tensor.h"
#include "../../LinAlg/sparse.h"

void print_tensor(tensor_t* t);
void print_tensor_verbose(tensor_t* t, int verbose);
void print_tensor_values(tensor_t* t);
void print_tensor_flat(tensor_t* t);
void print_shape(int* shape, int shape_dim);
void print_sparse(sparse* st);

#endif
