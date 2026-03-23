#ifndef FUNCTIONS
#define FUNCTIONS

#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Optim/constrained/nlp.h"


struct Quadratic
{
    tensor* A;
    tensor* b;
    float c;
};

struct Quadratic q;

void quadratic_init();
void quadratic_free();
float quadratic_eval(tensor* x);
void quadratic_eval2(tensor* x, tensor* out);
void quadratic_eval3(tensor* x, tensor* out);
struct nlp* get_nlp0();

#endif
