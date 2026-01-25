#include "newton.h"

#include <math.h>
#include <stdio.h>
#include "../../global.h"
#include "../../visual/prints/linalg.h"


struct optim_logs* newton(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    void (*delta2_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
)
{
    struct optim_logs* logs = new_optim_logs();

    float cost;
    #ifdef OPTIM_VERBOSE
    cost = cost_func(x0);
    printf("Initial cost: %.4f\n", cost);
    #endif

    // TODO: Should also output a final value for x.
    struct tensor* x = tensor_copy(x0);
    struct tensor* delta = tensor_copy_shape(x0);
    int delta2_shape[] = {delta->shape[0], delta->shape[0]};
    struct tensor* delta2 = new_tensor(delta2_shape, 2, NULL);
    struct tensor* delta2_inv = tensor_copy_shape(delta2);

    int total_steps = max_iters;
    for (int i = 0; i < max_iters; i++)
    {
        // Compute gradient
        delta_cost_func(x, delta);
        delta2_cost_func(x, delta2);

        // Compute Newton Step
        tensor_inverse(delta2, delta2_inv);
        tensor_mult(delta2_inv, delta, delta);

        // Stopping criterion
        float delta_magnitude = tensor_vec_magnitude(delta);
        if (delta_magnitude <= tolerance) {
            total_steps = i+1;
            break;
        }

        // Scale by alpha
        tensor_scalar_mult(delta, alpha, delta);
        tensor_sub(x, delta, x);

        #ifdef OPTIM_VERBOSE
        cost = cost_func(x);
        printf("Current Cost: %f\n", cost);
        optim_logs_add(logs, x, cost);
        #endif
    }
    #ifdef OPTIM_VERBOSE
    cost = cost_func(x);
    printf("Total steps taken: %d\n", total_steps);
    printf("Final cost: % .7f\n", cost);
    #endif

    logs->final_cost = cost;
    logs->final_x = x;

    tensor_free(delta);
    tensor_free(delta2);
    tensor_free(delta2_inv);
    return logs;
}
