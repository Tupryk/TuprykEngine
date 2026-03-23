#ifndef OPTIM_META
#define OPTIM_META

#include "../LinAlg/tensor.h"

// NOT OPTIMAL
struct optim_logs
{
    tensor* Xs;
    tensor* costs;
    tensor* final_x;
    float final_cost;
    int converged;
    int steps;
};

struct optim_logs* new_optim_logs();
void optim_logs_free(struct optim_logs* ol);
void optim_logs_add(struct optim_logs* ol, tensor* x, float cost);

#endif
