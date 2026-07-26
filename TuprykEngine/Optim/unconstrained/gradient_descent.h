#ifndef UNCONSTRAINED
#define UNCONSTRAINED

#include "../../ui/prints/linalg.h"

void gradient_descent_init(
    tensor_t* x0,
    float (*cost_func)(tensor_t*),
    void (*delta_cost_func)(tensor_t*, tensor_t*),
    float alpha,
    float tolerance,
    int max_iters
);
struct optim_logs* gradient_descent_run(tensor_t* x);
void gradient_descent_free();

#endif
