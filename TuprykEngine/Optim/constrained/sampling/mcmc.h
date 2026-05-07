#ifndef MCMC
#define MCMC

#include "../../../LinAlg/tensor.h"
// TODO: Move to another folder

void mcmc_sample(float (*P)(tensor*), tensor* x0, float step_std, int sample_count, tensor** outputs);

#endif