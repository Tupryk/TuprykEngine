#include <stdio.h>

#include "foam.h"


foam_t* foam_init(nlp_t* nlp, size_t max_foam, float inital_std)
{
    foam_t* foam = (foam_t*) malloc((sizeof(foam_t)));
    foam->dim = nlp->dim;
    foam->max_foam = max_foam;
    foam->inital_std = inital_std;
    foam->force_fields = stack_init();
    return foam;
}

void foam_add_particle(foam_t* foam, tensor* x)
{
    // Check if touching any other gaussian
    // TODO:

    if (foam->force_fields->size < foam->max_foam)
    {
        gaussian_t* gauss = (gaussian_t*) malloc(sizeof(gaussian_t));
        gauss->scaling = 1.f;
        gauss->mu = tensor_copy(x);
        gauss->cov = new_tensor_diagonal_uniform(foam->dim, foam->inital_std);
        stack_push(foam->force_fields, gauss);
    }
}

float foam_eval(foam_t* foam, nlp_t* nlp, tensor* x)
{
    float out = 0.f;
    struct stack_elem* se = foam->force_fields->next;
    tensor* x_diff = tensor_copy_shape(x);
    while (se != NULL)
    {
        gaussian_t* gauss = (gaussian_t*) se->data;
        tensor_sub(gauss->mu, x, x_diff);

        float dist = vector_squared_norm(x_diff) - foam->inital_std * foam->inital_std;
        if (dist < 0 && !nlp_feasible(nlp, x))
        {
            out += -dist;
        }
        se = se->next;
    }
    tensor_free(x_diff);
    return out;
}

void foam_eval2(foam_t* foam, nlp_t* nlp, tensor* x, tensor* out)
{
    tensor_fill(out, 0.f);
    struct stack_elem* se = foam->force_fields->next;
    tensor* x_diff = tensor_copy_shape(x);
    tensor* ball_grad = tensor_copy_shape(x);
    while (se != NULL)
    {
        gaussian_t* gauss = (gaussian_t*) se->data;
        tensor_sub(gauss->mu, x, x_diff);

        float dist = vector_squared_norm(x_diff) - foam->inital_std * foam->inital_std;
        if (dist < 0 && !nlp_feasible(nlp, x))
        {
            tensor_scalar_mult(x_diff, 2.f, ball_grad);
            tensor_add(out, ball_grad, out);
        }
        se = se->next;
    }
    tensor_free(x_diff);
    tensor_free(ball_grad);
}

void foam_free(foam_t* foam)
{
    while (foam->force_fields->next != NULL)
    {
        gaussian_t* gauss = (gaussian_t*) stack_pop(foam->force_fields);
        tensor_free(gauss->mu);
        tensor_free(gauss->cov);
        free(gauss);
    }
    free(foam->force_fields);
    free(foam);
}
