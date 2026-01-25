#include <stdio.h>
#include "functions.h"
#include "../LinAlg/tensor.h"
#include "../visual/prints/linalg.h"


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

float quadratic_eval(struct tensor* x)
{
    struct tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);

    struct tensor* dot0 = tensor_mult_give(q.A, x);
    struct tensor* dot1 = tensor_mult_give(x_T, dot0);
    struct tensor* dot2 = tensor_mult_give(q.b, x);
    
    float out = 0.5 * dot1->values[0] + dot2->values[0] + q.c;
    
    tensor_free(x_T);
    tensor_free(dot0);
    tensor_free(dot1);
    tensor_free(dot2);

    return out;
}

void quadratic_eval2(struct tensor* x, struct tensor* out)
{
    struct tensor* b_T = tensor_copy(q.b);
    tensor_transpose(b_T);

    tensor_mult(q.A, x, out);
    tensor_add(out, q.b, out);
    
    tensor_free(b_T);
}

void quadratic_eval3(struct tensor* x, struct tensor* out)
{
    tensor_transfer_values(out, q.A);
}
