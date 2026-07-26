#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "newton.h"

#include "../../global.h"
#include "../../ui/prints/linalg.h"


struct optim_logs* newton(
    tensor_t* x0,
    float (*cost_func)(tensor_t*),
    void (*delta_cost_func)(tensor_t*, tensor_t*),
    void (*delta2_cost_func)(tensor_t*, tensor_t*),
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
    tensor_t* x = tensor_copy(x0);
    tensor_t* J = tensor_copy_shape(x0);
    int H_shape[] = {J->shape[0], J->shape[0]};
    tensor_t* H = new_tensor(H_shape, 2, NULL);
    tensor_t* H_inv = tensor_copy_shape(H);

    // Make sure inverse exists
    tensor_t* damping = tensor_copy_shape(H);
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
        float J_magnitude = vector_norm(J);
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

struct gn_context
{
    float (*cost_func)(tensor_t*);
    void (*delta_cost_func)(tensor_t*, tensor_t*);
    
    float alpha;
    float tolerance;
    int total_steps;
    int max_iters;
    
    tensor_t* x;
    tensor_t* J;
    tensor_t* J_T;
    tensor_t* H;
    tensor_t* H_inv;
    tensor_t* damping;
    struct optim_logs* logs;
};

static struct gn_context* ctx;

void gauss_newton_init(
    tensor_t* x0,
    float (*cost_func)(tensor_t*),
    void (*delta_cost_func)(tensor_t*, tensor_t*),
    float alpha,
    float tolerance,
    int max_iters
) {
    ctx = (struct gn_context*) malloc(sizeof(struct gn_context));

    ctx->cost_func = cost_func;
    ctx->delta_cost_func = delta_cost_func;
    ctx->alpha = alpha;
    ctx->tolerance = tolerance;
    ctx->max_iters = max_iters;

    ctx->x = tensor_copy(x0);
    ctx->J = tensor_copy_shape(x0);
    ctx->J_T = tensor_copy_shape(x0);
    tensor_transpose(ctx->J_T);
    
    ctx->H = new_tensor_matrix(x0->shape[0], x0->shape[0], NULL);
    ctx->H_inv = tensor_copy_shape(ctx->H);
    ctx->damping = tensor_copy_shape(ctx->H);
    tensor_identity(ctx->damping);
    tensor_scalar_mult(ctx->damping, 1e-3, ctx->damping);

    ctx->logs = new_optim_logs();

    ctx->total_steps = 0;
}

struct optim_logs* gauss_newton_run(tensor_t* x)
{
    tensor_transfer_values(ctx->x, x);

    float cost;
    #ifdef OPTIM_VERBOSE
    cost = ctx->cost_func(ctx->x);
    #endif

    int total_steps = ctx->max_iters;
    ctx->logs->converged = 0;
    for (int i = 0; i < ctx->max_iters; i++)
    {
        // Compute gradient
        ctx->delta_cost_func(ctx->x, ctx->J);

        // Hessian approximation
        tensor_transfer_values(ctx->J_T, ctx->J);
        tensor_mult(ctx->J, ctx->J_T, ctx->H);
        tensor_scalar_mult(ctx->H, 2.f, ctx->H);
        tensor_add(ctx->H, ctx->damping, ctx->H);

        // Compute Newton Step
        tensor_inverse(ctx->H, ctx->H_inv);
        tensor_mult(ctx->H_inv, ctx->J, ctx->J);

        // Stopping criterion
        float J_magnitude = vector_norm(ctx->J);
        if (J_magnitude <= ctx->tolerance) {
            ctx->logs->converged = 1;
            total_steps = i+1;
            break;
        }

        // Scale by alpha
        tensor_scalar_mult(ctx->J, ctx->alpha, ctx->J);
        tensor_sub(ctx->x, ctx->J, ctx->x);

        #ifdef OPTIM_VERBOSE
        cost = ctx->cost_func(ctx->x);
        optim_logs_add(ctx->logs, ctx->x, cost);
        #endif
    }
    #ifdef OPTIM_VERBOSE
    cost = ctx->cost_func(ctx->x);
    #endif

    ctx->logs->final_cost = cost;
    ctx->logs->final_x = tensor_copy(ctx->x);

    return ctx->logs;
}

void gauss_newton_free()
{
    tensor_free(ctx->J);
    tensor_free(ctx->J_T);
    tensor_free(ctx->H);
    tensor_free(ctx->H_inv);
    tensor_free(ctx->damping);
    tensor_free(ctx->x);
    optim_logs_free(ctx->logs);
    free(ctx);
}
