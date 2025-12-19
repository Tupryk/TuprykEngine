#ifndef OPTIM_UNCONSTRAINED_NEWTON
#define OPTIM_UNCONSTRAINED_NEWTON

#include "../../visual/linalg.h"

float newton(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    void (*delta2_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
);

// void gauss_newton(
//     struct tensor* x0,
//     float (*cost_func)(struct tensor*),
//     void (*delta_cost_func)(struct tensor*, struct tensor*),
//     float alpha,
//     float tolerance,
//     int max_iters
// );

#endif
