#ifndef NLP
#define NLP

#include "../meta.h"
#include "../../LinAlg/tensor.h"


typedef float (*scalar_fn)(tensor*);
typedef void (*vector_fn)(tensor*, tensor*);

typedef struct
{
    int dim;
    int eq_count;
    int ineq_count;

    scalar_fn f;
    scalar_fn* eq;
    scalar_fn* ineq;

    vector_fn delta_f;
    vector_fn* delta_eq;
    vector_fn* delta_ineq;

    vector_fn delta2_f;
    vector_fn* delta2_eq;
    vector_fn* delta2_ineq;
} nlp_t;

int nlp_feasible(nlp_t* nlp, tensor* x);
void nlp_free(nlp_t* nlp);

struct nlp_optim_logs
{
    struct optim_logs* ol;
    tensor* final_x;
};

struct nlp_optim_logs* nlp_logs_init();
void nlp_logs_free(struct nlp_optim_logs* nl);

#endif
