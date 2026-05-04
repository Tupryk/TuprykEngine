#ifndef AUG_LAG
#define AUG_LAG

void aug_lagrangian_init(
    tensor* x0,
    nlp_t* nlp,
    float alpha,
    float tolerance,
    int max_outer_steps,
    int max_inner_steps
);
struct nlp_optim_logs* aug_lagrangian_run(tensor* x);
void aug_lagrangian_free();

#endif
