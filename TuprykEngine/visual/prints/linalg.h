#ifndef LINALG_VIS
#define LINALG_VIS

#include "../../LinAlg/tensor.h"

void print_tensor(struct tensor *t);
void print_tensor_verbose(struct tensor *t, int verbose);
void print_tensor_flat(struct tensor *t);
void print_shape(int* shape, int shape_dim);

#endif
