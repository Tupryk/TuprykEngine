#ifndef NLP
#define NLP

#include "../meta.h"
#include "../../LinAlg/tensor.h"


typedef float (*scalar_fn)(struct tensor*);
typedef void (*constraint_fn)(struct tensor*, struct tensor*);

struct nlp
{
    int eq_count;
    int ineq_count;

    scalar_fn f;
    scalar_fn* eq;
    scalar_fn* ineq;

    constraint_fn delta_f;
    constraint_fn* delta_eq;
    constraint_fn* delta_ineq;

    constraint_fn delta2_f;
    constraint_fn* delta2_eq;
    constraint_fn* delta2_ineq;
};

void nlp_free(struct nlp* nlp_);

struct nlp_optim_logs
{
    struct optim_logs* ol;
};

struct nlp_optim_logs* nlp_logs_init();
void nlp_logs_free(struct nlp_optim_logs* nl);

#endif
