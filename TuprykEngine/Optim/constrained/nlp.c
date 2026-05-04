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
        if (eval > 0) return 0;
    }

    for (int i = 0; i < nlp->eq_count; i++)
    {
        float eval = nlp->eq[i](x);
        if (fabsf(eval) > 1e-6) return 0;
    }

    return 1;
}

struct nlp_optim_logs* nlp_logs_init()
{
    struct nlp_optim_logs* nl = (struct nlp_optim_logs*) malloc(sizeof(struct nlp_optim_logs));
    nl->ol = new_optim_logs();
    nl->final_x = NULL;
    return nl;
}

void nlp_logs_free(struct nlp_optim_logs* nl)
{
    optim_logs_free(nl->ol);
    if (nl->final_x != NULL) tensor_free(nl->final_x);
    free(nl);
}
