#ifndef OPTIM_META
#define OPTIM_META

#include "../LinAlg/tensor.h"

#define EPSILON 1e-3

// NOT OPTIMAL
struct optim_logs
{
    tensor_t* Xs;
    tensor_t* costs;
    tensor_t* final_x;
    float final_cost;
    int converged;
    int steps;
};

struct optim_logs* new_optim_logs();
void optim_logs_free(struct optim_logs* ol);
void optim_logs_add(struct optim_logs* ol, tensor_t* x, float cost);

#endif
