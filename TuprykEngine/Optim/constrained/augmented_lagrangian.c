#include <stdio.h>
#include <stdlib.h>
#include "nlp.h"
#include "augmented_lagrangian.h"
#include "../unconstrained/gradient_descent.h"

struct auglag_context
{
    struct nlp* nlp_;

    struct tensor* nu;
    struct tensor* mu;

    struct nlp_optim_logs* logs;
};

static struct auglag_context* ctx;

float aug_lagrangian_eval_nlp(struct tensor* x)
{
    float out = ctx->nlp_->f(x);
    return out;
}

void aug_lagrangian_eval2_nlp(struct tensor* x, struct tensor* out)
{
    ctx->nlp_->delta_f(x, out);
}

void aug_lagrangian_init(
    struct tensor* x0,
    struct nlp* nlp_,
    float alpha,
    float tolerance,
    int max_iters
) {
    ctx = (struct auglag_context*) malloc(sizeof(struct auglag_context));
    ctx->logs = nlp_logs_init();
    ctx->nlp_ = nlp_;

    if (nlp_->eq_count > 0)
    {
        int nu_shape[] = {nlp_->eq_count, 1}; // TODO: check if nu/mu names are correct
        ctx->nu = new_tensor(nu_shape, 2, NULL);
    } else ctx->nu = NULL;

    if (nlp_->ineq_count > 0)
    {
        int mu_shape[] = {nlp_->ineq_count, 1};
        ctx->mu = new_tensor(mu_shape, 2, NULL);
    } else ctx->mu = NULL;

    gradient_descent_init(
        x0,
        aug_lagrangian_eval_nlp,
        aug_lagrangian_eval2_nlp,
        alpha,
        tolerance,
        max_iters
    );
}

int aug_lagrangian_run()
{
    gradient_descent_run();
    return ctx->logs->ol->converged;
}

void aug_lagrangian_free()
{
    nlp_logs_free(ctx->logs);
    gradient_descent_free();
    if (ctx->nu != NULL) tensor_free(ctx->nu);
    if (ctx->mu != NULL) tensor_free(ctx->mu);
    free(ctx);
}
