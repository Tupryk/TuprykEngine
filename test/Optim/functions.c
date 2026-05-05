#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/visual/prints/linalg.h"
#include "../../TuprykEngine/Optim/constrained/nlp.h"


void quadratic_init()
{
    int shape_A[] = {2, 2};
    float values_A[] = {
        1, 0,
        0, 1
    };
    q.A = new_tensor(shape_A, 2, values_A);

    int shape_b[] = {1, 2};
    float values_b[] = {
        -2,
        4
    };
    q.b = new_tensor(shape_b, 2, values_b);

    q.c = 5.f;
}

void quadratic_free()
{
    tensor_free(q.A);
    tensor_free(q.b);
}

float quadratic_eval(tensor* x)
{
    tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);

    tensor* dot0 = tensor_mult_give(q.A, x);
    tensor* dot1 = tensor_mult_give(x_T, dot0);
    tensor* dot2 = tensor_mult_give(q.b, x);
    
    float out = 0.5 * dot1->values[0] + dot2->values[0] + q.c;
    
    tensor_free(x_T);
    tensor_free(dot0);
    tensor_free(dot1);
    tensor_free(dot2);

    return out;
}

void quadratic_eval2(tensor* x, tensor* out)
{
    tensor* b_T = tensor_copy(q.b);
    tensor_transpose(b_T);

    tensor_mult(q.A, x, out);
    tensor_add(out, q.b, out);
    
    tensor_free(b_T);
}

void quadratic_eval3(tensor* x, tensor* out)
{
    tensor_transfer_values(out, q.A);
}

nlp_t* get_nlp0()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 0;
    nlp->ineq_count = 0;

    nlp->f = quadratic_eval;
    nlp->eq = NULL;
    nlp->ineq = NULL;
    
    nlp->delta_f = quadratic_eval2;
    nlp->delta_eq = NULL;
    nlp->delta_ineq = NULL;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}

float zero_eval(tensor* x) { return 0.f; }
void zero_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = 0.f; }

float side1_eval(tensor* x) { return x->values[0] - 0.5; }
void side1_eval2(tensor* x, tensor* out) { out->values[0] = 1.f; out->values[1] = 0.f; }

float side2_eval(tensor* x) { return -x->values[0] - 0.5; }
void side2_eval2(tensor* x, tensor* out) { out->values[0] = -1.f; out->values[1] = 0.f; }

float side3_eval(tensor* x) { return -x->values[1] - 0.5; }
void side3_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = -1.f; }

float side4_eval(tensor* x) { return x->values[1] - 0.5; }
void side4_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = 1.f; }

nlp_t* get_nlp1()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 0;
    nlp->ineq_count = 4;
    
    nlp->f = zero_eval;
    nlp->eq = NULL;
    nlp->ineq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->ineq_count);
    nlp->ineq[0] = side1_eval;
    nlp->ineq[1] = side2_eval;
    nlp->ineq[2] = side3_eval;
    nlp->ineq[3] = side4_eval;
    
    nlp->delta_f = zero_eval2;
    nlp->delta_eq = NULL;
    nlp->delta_ineq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->ineq_count);
    nlp->delta_ineq[0] = side1_eval2;
    nlp->delta_ineq[1] = side2_eval2;
    nlp->delta_ineq[2] = side3_eval2;
    nlp->delta_ineq[3] = side4_eval2;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}

float circle_eval(tensor* x) { return vector_squared_norm(x) - 0.5; }
void circle_eval2(tensor* x, tensor* out) { tensor_scalar_mult(x, 2, out); }

nlp_t* get_nlp2()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 0;
    nlp->ineq_count = 1;
    
    nlp->f = zero_eval;
    nlp->eq = NULL;
    nlp->ineq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->ineq_count);
    nlp->ineq[0] = circle_eval;
    
    nlp->delta_f = zero_eval2;
    nlp->delta_eq = NULL;
    nlp->delta_ineq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->ineq_count);
    nlp->delta_ineq[0] = circle_eval2;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}

nlp_t* get_nlp3()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 1;
    nlp->ineq_count = 0;
    
    nlp->f = zero_eval;
    nlp->eq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->eq_count);
    nlp->eq[0] = circle_eval;
    nlp->ineq = NULL;
    
    nlp->delta_f = zero_eval2;
    nlp->delta_eq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->eq_count);
    nlp->delta_eq[0] = circle_eval2;
    nlp->delta_ineq = NULL;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}

nlp_t* get_nlp4()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 1;
    nlp->ineq_count = 0;
    
    nlp->f = zero_eval;
    nlp->eq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->eq_count);
    nlp->eq[0] = side1_eval;
    nlp->ineq = NULL;
    
    nlp->delta_f = zero_eval2;
    nlp->delta_eq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->eq_count);
    nlp->delta_eq[0] = side1_eval2;
    nlp->delta_ineq = NULL;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}
