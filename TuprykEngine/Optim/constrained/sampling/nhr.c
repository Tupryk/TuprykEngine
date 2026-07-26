#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "nhr.h"
#include "../../../Algos/utils.h"
#include "../../../Stochastic/sample.h"

void nhr_sample(nlp_t* nlp, tensor_t* feasible_point, int sample_count, float delta_max, tensor_t** output)
{
    // TODO: Use nlp->eq
    tensor_t* last_point = tensor_copy(feasible_point);
    tensor_t* sampled_direction = tensor_copy_shape(feasible_point);
    tensor_t* y = tensor_copy_shape(feasible_point);
    tensor_t* g_delta = tensor_copy_shape(feasible_point);
    tensor_t* x_y_diff = tensor_copy_shape(feasible_point);

    for (int i = 0; i < sample_count; i++)
    {
        tensor_fill_gauss(sampled_direction, 0, 1);
        vector_normalize(sampled_direction);

        float beta_lo = -delta_max;
        float beta_up =  delta_max;

        while (1)
        {
            if (beta_lo >= beta_up) { i--; break; }

            float beta = rand_uni(beta_lo, beta_up);
            tensor_scalar_mult(sampled_direction, beta, y);
            tensor_add(y, last_point, y);
            
            float g_y[nlp->ineq_count];
            int all_feasible = 1;
            for (int j = 0; j < nlp->ineq_count; j++)
            {
                g_y[j] = nlp->ineq[j](y);
                if (g_y[j] > 0) all_feasible = 0;
            }

            if (all_feasible)
            {
                float acceptance_prob = min(1.f,
                    expf(-nlp->f(y)) / expf(-nlp->f(last_point))
                );
                if (nlp->f != NULL && acceptance_prob > rand_())
                {
                    tensor_transfer_values(last_point, y);
                    output[i] = tensor_copy(last_point);
                }
                else i--;
                break;
            }

            tensor_sub(last_point, y, x_y_diff);
            for (int j = 0; j < nlp->ineq_count; j++)
            {
                if (g_y[j] >= 0)
                {
                    nlp->delta_ineq[j](y, g_delta);
                    float g_line = g_y[j] + vector_dot(g_delta, x_y_diff);
                    float a = vector_dot(g_delta, sampled_direction);

                    beta_lo = max(beta_lo, -g_line/a);
                    beta_up = min(beta_up, -g_line/a);
                }
            }
        }
    }
    
    tensor_free(last_point);
    tensor_free(sampled_direction);
    tensor_free(y);
    tensor_free(g_delta);
    tensor_free(x_y_diff);
}
