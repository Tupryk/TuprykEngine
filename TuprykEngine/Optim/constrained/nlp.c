#include "nlp.h"
#include <stdlib.h>


void nlp_free(struct nlp* nlp_)
{
    if (nlp_->eq != NULL) free(nlp_->eq);
    if (nlp_->ineq != NULL) free(nlp_->ineq);

    if (nlp_->delta_eq != NULL) free(nlp_->delta_eq);
    if (nlp_->delta_ineq != NULL) free(nlp_->delta_ineq);

    if (nlp_->delta2_eq != NULL) free(nlp_->delta2_eq);
    if (nlp_->delta2_ineq != NULL) free(nlp_->delta2_ineq);

    free(nlp_);
}

struct nlp_optim_logs* nlp_logs_init()
{
    struct nlp_optim_logs* nl = (struct nlp_optim_logs*) malloc(sizeof(struct nlp_optim_logs));
    nl->ol = new_optim_logs();
    return nl;
}

void nlp_logs_free(struct nlp_optim_logs* nl)
{
    optim_logs_free(nl->ol);
    free(nl);
}
