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

    // TODO: Should also output a final value for x
    struct tensor* x = tensor_copy(x0);
    struct tensor* J = tensor_copy_shape(x0);
    int H_shape[] = {J->shape[0], J->shape[0]};
    struct tensor* H = new_tensor(H_shape, 2, NULL);
    struct tensor* H_inv = tensor_copy_shape(H);
    struct tensor* damping = tensor_copy_shape(H);
    tensor_identity(damping);
    tensor_scalar_mult(damping, 1e-3, damping);

    int total_steps = max_iters;
    for (int i = 0; i < max_iters; i++)
    {
        // Compute gradient
        delta_cost_func(x, J);
        delta2_cost_func(x, H);
        
        // Compute Newton Step
        tensor_add(H, damping, H);
        tensor_inverse(H, H_inv);
        tensor_mult(H_inv, J, J);

        // Stopping criterion
        float J_magnitude = tensor_vec_magnitude(J);
        if (J_magnitude <= tolerance) {
            total_steps = i+1;
            break;
        }

        // Scale by alpha
        tensor_scalar_mult(J, alpha, J);
        tensor_sub(x, J, x);

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

    tensor_free(J);
    tensor_free(H);
    tensor_free(H_inv);
    return logs;
}

struct optim_logs* gauss_newton(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
)
{
    /*
    Only works on squared costs!
    */

    struct optim_logs* logs = new_optim_logs();

    float cost;
    #ifdef OPTIM_VERBOSE
    cost = cost_func(x0);
    printf("Initial cost: %.4f\n", cost);
    #endif

    // TODO: Should also output a final value for x
    struct tensor* x = tensor_copy(x0);
    struct tensor* J = tensor_copy_shape(x0);
    struct tensor* J_T = tensor_copy_shape(x0);
    tensor_transpose(J_T);
    int H_shape[] = {J->shape[0], J->shape[0]};
    struct tensor* H = new_tensor(H_shape, 2, NULL);
    struct tensor* H_inv = tensor_copy_shape(H);
    struct tensor* damping = tensor_copy_shape(H);
    tensor_identity(damping);
    tensor_scalar_mult(damping, 1e-3, damping);

    int total_steps = max_iters;
    for (int i = 0; i < max_iters; i++)
    {
        // Compute gradient
        delta_cost_func(x, J);

        // Hessian approximation
        tensor_transfer_values(J_T, J);
        // TODO: Sparse Hessian?
        tensor_mult(J, J_T, H);
        tensor_scalar_mult(H, 2.f, H);
        tensor_add(H, damping, H);

        // Compute Newton Step
        tensor_inverse(H, H_inv);
        tensor_mult(H_inv, J, J);

        // Stopping criterion
        float J_magnitude = tensor_vec_magnitude(J);
        if (J_magnitude <= tolerance) {
            total_steps = i+1;
            break;
        }

        // Scale by alpha
        tensor_scalar_mult(J, alpha, J);
        tensor_sub(x, J, x);

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

    tensor_free(J);
    tensor_free(J_T);
    tensor_free(H);
    tensor_free(H_inv);
    return logs;
}
