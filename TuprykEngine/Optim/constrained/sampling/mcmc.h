#ifndef MCMC
#define MCMC

#include "../../../LinAlg/tensor.h"
// TODO: Move to another folder

void mcmc_sample(float (*P)(tensor_t*), tensor_t* x0, float step_std, int sample_count, tensor_t** outputs);

#endif