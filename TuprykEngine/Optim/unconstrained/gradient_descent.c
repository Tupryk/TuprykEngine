#include "gradient_descent.h"

#include <math.h>
#include <stdio.h>
#include "../../global.h"
#include "../../visual/linalg.h"


float gradient_descent(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters) {
    
    float cost;
    #ifdef OPTIM_VERBOSE
    cost = cost_func(x0);
    printf("Initial cost: %.4f\n", cost);
    #endif

    // TODO: Should also output a final value for x.
    struct tensor* x = tensor_copy(x0);
    struct tensor* delta = tensor_copy(x0);
    int total_steps = max_iters;
    for (int i = 0; i < max_iters; i++)
    {
        // Compute gradient
        delta_cost_func(x, delta);
        // TODO: Sort this out
        float delta_magnitude = 0.f;
        for (int i = 0; i < delta->shape[0]; i++) {
            // float delta_magnitude = tensor_vec_magnitude(&delta);
            delta_magnitude += delta->values[i] * delta->values[i];
        }
        delta_magnitude = sqrt(delta_magnitude);
        
        // Stopping criterion
        if (delta_magnitude <= tolerance) {
            total_steps = i+1;
            break;
        }

        // Scale by alpha
        tensor_scalar_mult(delta, alpha, delta);
        tensor_sub(x, delta, x);

        #ifdef OPTIM_VERBOSE
        cost = cost_func(x);
        // printf("delta: ");
        // print_tensor(delta);
        printf("Current Cost: %f\n", cost);
        // printf("x: ");
        // print_tensor(x);
        #endif
    }
    #ifdef OPTIM_VERBOSE
    cost = cost_func(x);
    printf("Total steps taken: %d\n", total_steps);
    printf("Final cost: % .7f\n", cost);
    #endif

    free_tensor(x);
    free_tensor(delta);
    return cost;
}
