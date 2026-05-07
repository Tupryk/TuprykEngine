#include "mcmc.h"
#include "../../../Algos/utils.h"
#include "../../../Stochastic/sample.h"


void mcmc_sample(float (*P)(tensor*), tensor* x0, float step_std, int sample_count, tensor** outputs)
{
    // Metropolis-Hastings
    tensor* x = tensor_copy(x0);
    tensor* x_delta = tensor_copy_shape(x0);
    tensor* noise = tensor_copy_shape(x0);
    int burn_in = 1000;
    for (int i = -burn_in; i < sample_count; i++)
    {
        tensor_fill_gauss(noise, 0.f, step_std);
        tensor_add(x, noise, x_delta);

        float acceptance_ratio = min(1.f, P(x_delta) / P(x));
        if (acceptance_ratio <= rand_()) { i--; continue; }  // Reject
        tensor_transfer_values(x, x_delta);

        if (i >= 0)
        {
            outputs[i] = tensor_copy(x);
        }
    }
    tensor_free(x);
    tensor_free(x_delta);
    tensor_free(noise);
}
