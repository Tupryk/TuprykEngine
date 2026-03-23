#ifndef UNCONSTRAINED
#define UNCONSTRAINED

#include "../../visual/prints/linalg.h"

void gradient_descent_init(
    tensor* x0,
    float (*cost_func)(tensor*),
    void (*delta_cost_func)(tensor*, tensor*),
    float alpha,
    float tolerance,
    int max_iters
);
int gradient_descent_run();
void gradient_descent_free();

#endif
