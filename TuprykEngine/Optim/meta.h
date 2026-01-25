#ifndef OPTIM_META
#define OPTIM_META

#include "../LinAlg/tensor.h"

// NOT OPTIMAL
struct optim_logs
{
    struct tensor* Xs;
    struct tensor* costs;
    int steps;
    float final_cost;
    struct tensor* final_x;
};

struct optim_logs* new_optim_logs();
void optim_logs_free(struct optim_logs* ol);
void optim_logs_add(struct optim_logs* ol, struct tensor* x, float cost);

#endif
