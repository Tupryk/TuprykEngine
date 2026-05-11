#include <math.h>
#include <stdlib.h>

#include "nlp.h"


void nlp_free(nlp_t* nlp)
{
    if (nlp->eq != NULL) free(nlp->eq);
    if (nlp->ineq != NULL) free(nlp->ineq);

    if (nlp->delta_eq != NULL) free(nlp->delta_eq);
    if (nlp->delta_ineq != NULL) free(nlp->delta_ineq);

    if (nlp->delta2_eq != NULL) free(nlp->delta2_eq);
    if (nlp->delta2_ineq != NULL) free(nlp->delta2_ineq);

    free(nlp);
}

int nlp_feasible(nlp_t* nlp, tensor* x)
{
    for (int i = 0; i < nlp->ineq_count; i++)
    {
        float eval = nlp->ineq[i](x);
        if (eval > 1e-2) return 0;
    }

    for (int i = 0; i < nlp->eq_count; i++)
    {
        float eval = nlp->eq[i](x);
        if (fabsf(eval) > 1e-2) return 0;
    }

    return 1;
}

float nlp_infeasible_cost(nlp_t* nlp, tensor* x)
{
    float cost = 0.f;
    for (int i = 0; i < nlp->ineq_count; i++)
    {
        float eval = nlp->ineq[i](x);
        if (eval > 1e-2) cost += eval;
    }

    for (int i = 0; i < nlp->eq_count; i++)
    {
        float eval = nlp->eq[i](x);
        if (fabsf(eval) > 1e-2) cost += eval;
    }

    return cost;
}

struct nlp_optim_logs* nlp_logs_init()
{
    struct nlp_optim_logs* nl = (struct nlp_optim_logs*) malloc(sizeof(struct nlp_optim_logs));
    nl->ol = new_optim_logs();
    nl->final_x = NULL;
    return nl;
}

float nlp_sos_const_eval(nlp_t* nlp, tensor* x)
{
    float out = nlp->f != NULL ? nlp->f(x) : 0.f;

    for (int i = 0; i < nlp->ineq_count; i++)
    {
        float gi = nlp->ineq[i](x);
        if (gi > 0) out += gi * gi * 0.5f;
    }

    for (int i = 0; i < nlp->eq_count; i++)
    {
        float hi = nlp->eq[i](x);
        out += hi * hi * 0.5f;
    }

    return out;
}

void nlp_sos_const_eval2(nlp_t* nlp, tensor* x, tensor* out)
{
    if (nlp->delta_f != NULL) nlp->delta_f(x, out);
    else tensor_fill(out, 0.f);

    tensor* eval2 = tensor_copy_shape(x);

    for (int i = 0; i < nlp->ineq_count; i++)
    {
        float gi = nlp->ineq[i](x);
        if (gi > 0)
        {
            nlp->delta_ineq[i](x, eval2);
            tensor_add(out, eval2, out);
        }
    }

    for (int i = 0; i < nlp->eq_count; i++)
    {
        nlp->delta_eq[i](x, eval2);
        tensor_add(out, eval2, out);
    }

    tensor_free(eval2);
}

void nlp_logs_free(struct nlp_optim_logs* nl)
{
    optim_logs_free(nl->ol);
    if (nl->final_x != NULL) tensor_free(nl->final_x);
    free(nl);
}
