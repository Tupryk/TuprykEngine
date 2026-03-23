#ifndef OPTIM_UNCONSTRAINED_NEWTON
#define OPTIM_UNCONSTRAINED_NEWTON

#include "../meta.h"
#include "../../visual/prints/linalg.h"

struct optim_logs* newton(
    tensor* x0,
    float (*cost_func)(tensor*),
    void (*delta_cost_func)(tensor*, tensor*),
    void (*delta2_cost_func)(tensor*, tensor*),
    float alpha,
    float tolerance,
    int max_iters
);

struct optim_logs* gauss_newton(
    tensor* x0,
    float (*cost_func)(tensor*),
    void (*delta_cost_func)(tensor*, tensor*),
    float alpha,
    float tolerance,
    int max_iters
);

#endif
