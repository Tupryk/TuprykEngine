#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "nlp.h"
#include "../meta.h"
#include "augmented_lagrangian.h"
#include "../unconstrained/gradient_descent.h"


struct auglag_context
{
    tensor* x;

    nlp_t* nlp;

    tensor* lambda;
    tensor* kappa;
    float mu;
    float nu;
    int max_outer_steps;

    struct nlp_optim_logs* logs;
};

static struct auglag_context* ctx;

float aug_lagrangian_eval_nlp(tensor* x)
{
    float out = ctx->nlp->f(x);

    float ineq_squared_sum = 0.f;
    for (int i = 0; i < ctx->nlp->ineq_count; i++)
    {
        float eval = ctx->nlp->ineq[i](x);
        ineq_squared_sum += eval > 0.f ? eval * eval : 0.f;
        out += ctx->lambda->values[i] * eval;
    }
    out += 0.5f * ctx->mu * ineq_squared_sum;

    return out;
}

void aug_lagrangian_eval2_nlp(tensor* x, tensor* out)
{
    ctx->nlp->delta_f(x, out);

    tensor* eval2 = tensor_copy_shape(x);

    for (int i = 0; i < ctx->nlp->ineq_count; i++)
    {
        float eval = ctx->nlp->ineq[i](x);
        ctx->nlp->delta_ineq[i](x, eval2);
        eval = eval > 0.f ? ctx->mu * eval : 0.f;
        tensor_scalar_mult(eval2, ctx->lambda->values[i] + eval, eval2);
        tensor_add(out, eval2, out);
    }

    tensor_free(eval2);
}

void aug_lagrangian_init(
    tensor* x0,
    nlp_t* nlp,
    float alpha,
    float tolerance,
    int max_outer_steps,
    int max_inner_steps
) {
    ctx = (struct auglag_context*) malloc(sizeof(struct auglag_context));
    ctx->logs = nlp_logs_init();
    ctx->logs->final_x = tensor_copy(x0);
    ctx->nlp = nlp;
    ctx->x = tensor_copy(x0);

    if (nlp->ineq_count > 0) {
        ctx->lambda = new_tensor_vector(nlp->ineq_count, NULL);
    } else ctx->lambda = NULL;

    if (nlp->eq_count > 0) {
        ctx->kappa = new_tensor_vector(nlp->eq_count, NULL);
    } else ctx->kappa = NULL;

    ctx->mu = 1.f;
    ctx->nu = 1.f;
    ctx->max_outer_steps = max_outer_steps;

    gradient_descent_init(
        x0,
        aug_lagrangian_eval_nlp,
        aug_lagrangian_eval2_nlp,
        alpha,
        tolerance,
        max_inner_steps
    );
}

struct nlp_optim_logs* aug_lagrangian_run(tensor* x)
{
    tensor_transfer_values(ctx->x, x);

    for (int i = 0; i < ctx->max_outer_steps; i++)
    {
        struct optim_logs* ol = gradient_descent_run(ctx->x);
        
        // Augmented Lagrangian dual updates
        for (int j = 0; j < ctx->nlp->ineq_count; j++)
        {
            float gi = ctx->nlp->ineq[j](ctx->x);
            float lambda = ctx->lambda->values[j];
            float tmp = lambda + ctx->mu * gi;
            ctx->lambda->values[j] = tmp > 0.f ? tmp : 0.f;
        }
    
        tensor_transfer_values(ctx->x, ol->final_x);
        tensor_transfer_values(ctx->logs->final_x, ctx->x);
    }
    
    return ctx->logs;
}

void aug_lagrangian_free()
{
    nlp_logs_free(ctx->logs);
    gradient_descent_free();
    if (ctx->lambda != NULL) tensor_free(ctx->lambda);
    if (ctx->kappa != NULL) tensor_free(ctx->kappa);
    tensor_free(ctx->x);
    free(ctx);
}
