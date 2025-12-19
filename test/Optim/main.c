#define DEBUG
#define OPTIM_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include "../../TuprykEngine/visual/linalg.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Optim/unconstrained/newton.h"
#include "../../TuprykEngine/Optim/unconstrained/gradient_descent.h"


// TODO: add a folder/file with many toy fuctions such as this one
struct Quadratic
{
    struct tensor* H;
    struct tensor* v;
    float c;
};

struct Quadratic q;

void quadratic_init()
{
    int shape_H[] = {2, 2};
    float values_H[] = {2.f, -2.f, 1.f, -3.f};
    q.H = new_tensor(shape_H, 2, values_H);

    int shape_v[] = {2};
    float values_v[] = {4.f, -1.f};
    q.v = new_tensor(shape_v, 1, values_v);
    tensor_transpose(q.v);

    q.c = 0.f;
}

void quadratic_free()
{
    free_tensor(q.H);
    free_tensor(q.v);
}

float quadratic_eval(struct tensor* x)
{
    struct tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);

    struct tensor* dot0 = tensor_mult_give(q.H, x_T);
    struct tensor* dot = tensor_mult_give(x, dot0);
    struct tensor* a = tensor_mult_give(x, q.v);
    
    float out = dot->values[0] + a->values[0] + q.c;
    
    free_tensor(x_T);
    free_tensor(dot0);
    free_tensor(dot);
    free_tensor(a);

    return out;
}

void quadratic_eval2(struct tensor* x, struct tensor* out)
{
    tensor_mult(q.H, x, out);
    tensor_scalar_mult(out, 2.f, out);
}

void quadratic_eval3(struct tensor* x, struct tensor* out)
{
    out->values[0] =  2.f;
    out->values[1] = -2.f;
    out->values[2] =  1.f;
    out->values[3] = -3.f;

    tensor_scalar_mult(out, 2.f, out);
}

int main()
{
    int shape[] = {2};
    TYPE values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 1, values);
    print_tensor(init_x);
    
    quadratic_init();

    printf("--- Test 1: Gradient Descent ---\n");
    gradient_descent(init_x, quadratic_eval, quadratic_eval2, 1e-2, 1e-2, 100);
    
    printf("--- Test 2: Newton ---\n");
    newton(init_x, quadratic_eval, quadratic_eval2, quadratic_eval3, 1e-2, 1e-2, 100);

    free_tensor(init_x);

    quadratic_free();

    return 0;
}
