#ifndef OPTIM_UNCONSTRAINED_NEWTON
#define OPTIM_UNCONSTRAINED_NEWTON

#include "../meta.h"
#include "../../ui/prints/linalg.h"

struct optim_logs* newton(
    tensor_t* x0,
    float (*cost_func)(tensor_t*),
    void (*delta_cost_func)(tensor_t*, tensor_t*),
    void (*delta2_cost_func)(tensor_t*, tensor_t*),
    float alpha,
    float tolerance,
    int max_iters
);

void gauss_newton_init(
    tensor_t* x0,
    float (*cost_func)(tensor_t*),
    void (*delta_cost_func)(tensor_t*, tensor_t*),
    float alpha,
    float tolerance,
    int max_iters
);

struct optim_logs* gauss_newton_run(tensor_t* x);

void gauss_newton_free();

#endif
