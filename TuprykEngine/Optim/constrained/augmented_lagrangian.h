#ifndef AUG_LAG
#define AUG_LAG

void aug_lagrangian_init(
    struct tensor* x0,
    struct nlp* nlp_,
    float alpha,
    float tolerance,
    int max_iters
);
int aug_lagrangian_run();
void aug_lagrangian_free();

#endif
