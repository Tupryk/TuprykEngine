#include "gradient_descent.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "../meta.h"
#include "../../global.h"
#include "../../visual/prints/linalg.h"


struct gd_context
{
    float (*cost_func)(struct tensor*);
    void (*delta_cost_func)(struct tensor*, struct tensor*);
    
    float alpha;
    float tolerance;
    int total_steps;
    int max_iters;
    
    struct tensor* x;
    struct tensor* J;
    struct optim_logs* logs;
};

static struct gd_context* ctx;

void gradient_descent_init(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
) {
    ctx = (struct gd_context*) malloc(sizeof(struct gd_context));

    ctx->cost_func = cost_func;
    ctx->delta_cost_func = delta_cost_func;
    ctx->alpha = alpha;
    ctx->tolerance = tolerance;
    ctx->max_iters = max_iters;

    ctx->x = tensor_copy(x0);
    ctx->J = tensor_copy_shape(x0);
    ctx->logs = new_optim_logs();

    ctx->total_steps = 0;
}

int gradient_descent_run()
{
    float cost;
    #ifdef OPTIM_VERBOSE
    cost = ctx->cost_func(ctx->x);
    printf("Initial cost: %.4f\n", cost);
    #endif

    int total_steps = ctx->max_iters;
    ctx->logs->converged = 0;
    for (int i = 0; i < ctx->max_iters; i++)
    {
        // Compute gradient
        ctx->delta_cost_func(ctx->x, ctx->J);
        
        // Stopping criterion
        float J_magnitude = tensor_norm(ctx->J);
        if (J_magnitude <= ctx->tolerance) {
            ctx->logs->converged = 1;
            total_steps = i+1;
            
            #ifdef OPTIM_VERBOSE
            printf("Converged with ||J||: %f\n", J_magnitude);
            #endif
            
            break;
        }
        
        // Scale by alpha
        tensor_scalar_mult(ctx->J, ctx->alpha, ctx->J);
        tensor_sub(ctx->x, ctx->J, ctx->x);
        
        #ifdef OPTIM_VERBOSE
        cost = ctx->cost_func(ctx->x);
        printf("Current Cost: %f\n", cost);
        optim_logs_add(ctx->logs, ctx->x, cost);
        #endif
    }
    #ifdef OPTIM_VERBOSE
    cost = ctx->cost_func(ctx->x);
    printf("Total steps taken: %d\n", total_steps);
    printf("Final cost: % .7f\n", cost);
    printf("x:\n");
    print_tensor(ctx->x);
    #endif
    
    ctx->logs->final_cost = cost;
    ctx->logs->final_x = tensor_copy(ctx->x);

    return ctx->logs->converged;
}

void gradient_descent_free()
{
    tensor_free(ctx->x);
    tensor_free(ctx->J);
    optim_logs_free(ctx->logs);
    free(ctx);
}
