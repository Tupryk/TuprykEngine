#ifndef NLP
#define NLP

#include "../meta.h"
#include "../../LinAlg/tensor.h"


typedef float (*scalar_fn)(tensor_t*);
typedef void (*vector_fn)(tensor_t*, tensor_t*);

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

int nlp_feasible(nlp_t* nlp, tensor_t* x);
float nlp_infeasible_cost(nlp_t* nlp, tensor_t* x);
void nlp_free(nlp_t* nlp);

struct nlp_optim_logs
{
    struct optim_logs* ol;
    tensor_t* final_x;
};

struct nlp_optim_logs* nlp_logs_init();
float nlp_sos_const_eval(nlp_t* nlp, tensor_t* x);
void nlp_sos_const_eval2(nlp_t* nlp, tensor_t* x, tensor_t* out);
void nlp_logs_free(struct nlp_optim_logs* nl);

#endif
