#ifndef FUNCTIONS
#define FUNCTIONS

#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Optim/constrained/nlp.h"


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
struct nlp* get_nlp0();

#endif
