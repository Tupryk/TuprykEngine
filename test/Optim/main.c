#define DEBUG
#define OPTIM_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include "../../TuprykEngine/visual/prints/linalg.h"
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
    float values_H[] = {
        2, 1,
        1, 3
    };
    q.H = new_tensor(shape_H, 2, values_H);

    int shape_v[] = {2};
    float values_v[] = {
         1,
        -2
    };
    q.v = new_tensor(shape_v, 1, values_v);
    tensor_transpose(q.v);

    q.c = 0.f;
}

void quadratic_free()
{
    tensor_free(q.H);
    tensor_free(q.v);
}

float quadratic_eval(struct tensor* x)
{
    struct tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);

    struct tensor* dot0 = tensor_mult_give(q.H, x_T);
    struct tensor* dot = tensor_mult_give(x, dot0);
    struct tensor* a = tensor_mult_give(x, q.v);
    
    float out = 0.5 * dot->values[0] + a->values[0] + q.c;
    
    tensor_free(x_T);
    tensor_free(dot0);
    tensor_free(dot);
    tensor_free(a);

    return out;
}

void quadratic_eval2(struct tensor* x, struct tensor* out)
{
    tensor_mult(q.H, x, out);
}

void quadratic_eval3(struct tensor* x, struct tensor* out)
{
    tensor_transfer_values(out, q.H);
}

int main()
{
    int shape[] = {2};
    TYPE values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 1, values);
    print_tensor(init_x);
    
    quadratic_init();

    printf("--- Test 1: Gradient Descent ---\n");
    gradient_descent(init_x, quadratic_eval, quadratic_eval2, 1e-1, 1e-2, 100);
    
    printf("--- Test 2: Newton ---\n");
    newton(init_x, quadratic_eval, quadratic_eval2, quadratic_eval3, 1, 1e-2, 100);

    tensor_free(init_x);

    quadratic_free();

    return 0;
}
