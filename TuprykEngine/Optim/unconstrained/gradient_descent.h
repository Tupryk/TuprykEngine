#ifndef UNCONSTRAINED
#define UNCONSTRAINED

#include "../../visual/prints/linalg.h"

void gradient_descent_init(
    struct tensor* x0,
    float (*cost_func)(struct tensor*),
    void (*delta_cost_func)(struct tensor*, struct tensor*),
    float alpha,
    float tolerance,
    int max_iters
);
int gradient_descent_run();
void gradient_descent_free();

#endif
