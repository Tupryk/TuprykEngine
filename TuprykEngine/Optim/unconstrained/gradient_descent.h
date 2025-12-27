#ifndef UNCONSTRAINED
#define UNCONSTRAINED

#include "../../visual/prints/linalg.h"

float gradient_descent(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
);

#endif
