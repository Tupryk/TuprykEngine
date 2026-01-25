#ifndef FUNCTIONS
#define FUNCTIONS

#include "../LinAlg/tensor.h"


struct Quadratic
{
    struct tensor* A;
    struct tensor* b;
    float c;
};

struct Quadratic q;

void quadratic_init();
void quadratic_free();
float quadratic_eval(struct tensor* x);
void quadratic_eval2(struct tensor* x, struct tensor* out);
void quadratic_eval3(struct tensor* x, struct tensor* out);

#endif
