#ifndef FUNCTIONS
#define FUNCTIONS

#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Optim/constrained/nlp.h"


struct Quadratic
{
    tensor_t* A;
    tensor_t* b;
    float c;
};

struct Quadratic q;

void quadratic_init();
void quadratic_free();
float quadratic_eval(tensor_t* x);
void quadratic_eval2(tensor_t* x, tensor_t* out);
void quadratic_eval3(tensor_t* x, tensor_t* out);
nlp_t* get_nlp0();
nlp_t* get_nlp1();
nlp_t* get_nlp2();
nlp_t* get_nlp3();
nlp_t* get_nlp4();
nlp_t* get_nlp_two_disks();
nlp_t* get_nlp_swiss_roll();
nlp_t* get_mod_circles();
nlp_t* get_cone();
float gaussian(tensor_t* x);

#endif
