#define DEBUG
// #define OPTIM_VERBOSE

#include <stdio.h>
#include "../functions.h"
#include "../../../TuprykEngine/Optim/meta.h"
#include "../../../TuprykEngine/Algos/lists.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Optim/constrained/nlp.h"
#include "../../../TuprykEngine/Optim/unconstrained/newton.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/nhr.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/mcmc.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/masem.h"
#include "../../../TuprykEngine/Optim/constrained/augmented_lagrangian.h"

#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/visual/graphics/basic.h"
#include "../../../TuprykEngine/visual/graphics/window.h"


nlp_t* (*get_global_nlp)() = get_mod_circles;

float square_side = 10.f;

void draw_x(nlp_t* nlp, tensor* x)
{
    int feasible = nlp_feasible(nlp, x);

    if (feasible) set_color(0.1216f, 0.4667f, 0.7059f);
    else set_color(1.0f, 0.4980f, 0.0549f);

    int cx = ((x->values[0] + square_side) / (square_side*2)) * ((float) WINDOW_W);
    int cy = ((-x->values[1] + square_side) / (square_side*2)) * ((float) WINDOW_H);
    draw_circle(cx, cy, 5);
}

int test_nlp_feasibility()
{
    int sample_count = 10000;
    tensor* x = new_tensor_vector(2, NULL);
    tensor* samples[sample_count];
    float max_cost = 0.f;
    
    nlp_t* nlp = get_global_nlp();

    printf("--- Test 2: NLP Feasibility ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        tensor_fill_uniform(x, -square_side, square_side);
        
        max_cost = fmaxf(max_cost, nlp_infeasible_cost(nlp, x));
        samples[i] = tensor_copy(x);
    }

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    for (int i = 0; i < sample_count; i++)
    {
        if (nlp_feasible(nlp, samples[i])) set_color(0.f, .8f, 0.f);
        else 
        {
            float c = nlp_infeasible_cost(nlp, samples[i]) / max_cost;
            set_color(c, 0.f, 1.f - c);
        }

        int cx = ((samples[i]->values[0] + square_side) / (square_side*2)) * ((float) WINDOW_W);
        int cy = ((-samples[i]->values[1] + square_side) / (square_side*2)) * ((float) WINDOW_H);
        draw_circle(cx, cy, 5);

        tensor_free(samples[i]);
    }

    window_wait();
    free_window();
    
    nlp_free(nlp);
    tensor_free(x);

    return 0;
}

nlp_t* nlp_ctx;

stack* foam;
float foam_radius = .5f;
const size_t max_foam = 250;

float nlp_sos_const_eval_ctx(tensor* x)
{
    float out = 0.f;
    struct stack_elem* foam_elem = foam->next;
    tensor* x_diff = tensor_copy_shape(x);
    while (foam_elem != NULL)
    {
        tensor* circle_center = (tensor*) foam_elem->data;
        tensor_sub(circle_center, x, x_diff);
        float dist = vector_squared_norm(x_diff) - foam_radius*foam_radius;
        if (dist < 0 && !nlp_feasible(nlp_ctx, x))
        {
            out += -dist;
        }
        foam_elem = foam_elem->next;
    }
    tensor_free(x_diff);
    out += nlp_sos_const_eval(nlp_ctx, x);
    return out;
}

void nlp_sos_const_eval2_ctx(tensor* x, tensor* out)
{
    tensor_fill(out, 0.f);
    struct stack_elem* foam_elem = foam->next;
    tensor* x_diff = tensor_copy_shape(x);
    tensor* ball_grad = tensor_copy_shape(x);
    while (foam_elem != NULL)
    {
        tensor* circle_center = (tensor*) foam_elem->data;
        tensor_sub(circle_center, x, x_diff);
        float dist = vector_squared_norm(x_diff) - foam_radius*foam_radius;
        if (dist < 0 && !nlp_feasible(nlp_ctx, x))
        {
            tensor_scalar_mult(x_diff, 2.f, ball_grad);
            tensor_add(out, ball_grad, out);
        }
        foam_elem = foam_elem->next;
    }
    tensor_free(x_diff);
    tensor_free(ball_grad);

    tensor* nlp_eval2 = tensor_copy_shape(out);
    nlp_sos_const_eval2(nlp_ctx, x, nlp_eval2);
    tensor_add(out, nlp_eval2, out);
    tensor_free(nlp_eval2);
}

int test_constraint_sampling()
{
    int sample_count = 5000;
    tensor* x = new_tensor_vector(2, NULL);
    tensor* samples[sample_count];
    foam = stack_init();
    
    nlp_ctx = get_global_nlp();

    int feasible_count = 0;
    int infeasible_count = 0;
    printf("--- Test 3: Constraint Sampling ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        if ((i+1) % 1000 == 0)
            printf("Transporting point %d of %d...\n", i+1, sample_count);
        tensor_fill_uniform(x, -square_side, square_side);
        
        gauss_newton_init(x, nlp_sos_const_eval_ctx, nlp_sos_const_eval2_ctx, 1, 1e-2, 500);
        struct optim_logs* logs = gauss_newton_run(x);
        tensor_transfer_values(x, logs->final_x);
        gauss_newton_free();

        samples[i] = tensor_copy(x);
        if (nlp_feasible(nlp_ctx, x)) feasible_count++;
        else
        {
            infeasible_count++;
            if (foam->size < max_foam)
            {
                tensor* ball_center = tensor_copy(x);
                stack_push(foam, ball_center);
            }
        }
    }

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    while (foam->size > 0)
    {
        tensor* ball_center = stack_pop(foam);

        int cx = ((ball_center->values[0] + square_side) / (square_side*2)) * ((float) WINDOW_W);
        int cy = ((-ball_center->values[1] + square_side) / (square_side*2)) * ((float) WINDOW_H);
        set_color(1.f, 0.f, 1.f);
        draw_circle(cx, cy, WINDOW_W * foam_radius / (square_side*2));
        
        tensor_free(ball_center);
    }
    stack_free(foam);

    for (int i = 0; i < sample_count; i++)
    {
        draw_x(nlp_ctx, samples[i]);
        tensor_free(samples[i]);
    }
    printf("Feasible Count: %d; Infeasible Count: %d\n", feasible_count, infeasible_count);

    window_wait();
    free_window();
    
    nlp_free(nlp_ctx);
    tensor_free(x);

    return 0;
}

int main()
{
    int failure_count = 0;

    failure_count += test_nlp_feasibility();
    failure_count += test_constraint_sampling();
    
    if (failure_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failure_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
