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
#include "../../../TuprykEngine/Optim/constrained/sampling/foam.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/masem.h"
#include "../../../TuprykEngine/Optim/constrained/augmented_lagrangian.h"

#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/visual/graphics/basic.h"
#include "../../../TuprykEngine/visual/graphics/window.h"


nlp_t* (*get_global_nlp)() = get_mod_circles;
nlp_t* nlp_ctx;
foam_t* foam_ctx;

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

float foam_eval_ctx(tensor* x) { return foam_eval(foam_ctx, nlp_ctx, x) + nlp_sos_const_eval(nlp_ctx, x); }
void foam_eval2_ctx(tensor* x, tensor* out)
{
    tensor* foam_out = tensor_copy_shape(out);
    foam_eval2(foam_ctx, nlp_ctx, x, foam_out);

    tensor* nlp_out = tensor_copy_shape(out);
    nlp_sos_const_eval2(nlp_ctx, x, nlp_out);

    tensor_add(foam_out, nlp_out, out);
    tensor_free(foam_out);
    tensor_free(nlp_out);
}

int test_foam_constraint_sampling()
{
    printf("--- Test 1: Foam Constraint Sampling ---\n");
    
    int sample_count = 1000;
    
    tensor* x = new_tensor_vector(2, NULL);
    
    nlp_ctx = get_global_nlp();
    foam_ctx = foam_init(nlp_ctx, 50, .5f, 10.f);
    
    tensor* samples[sample_count];

    //--------- Sampling ---------//
    int feasible_count = 0;
    int infeasible_count = 0;
    for (int i = 0; i < sample_count; i++)
    {
        if ((i+1) % 1000 == 0)
        {
            printf("Transporting point %d of %d...\n", i+1, sample_count);
        }
        tensor_fill_uniform(x, -square_side, square_side);
        
        gauss_newton_init(x, foam_eval_ctx, foam_eval2_ctx, 1, 1e-2, 500);
        struct optim_logs* logs = gauss_newton_run(x);
        tensor_transfer_values(x, logs->final_x);
        gauss_newton_free();

        samples[i] = tensor_copy(x);
        if (nlp_feasible(nlp_ctx, x)) feasible_count++;
        else
        {
            infeasible_count++;
            foam_add_particle(foam_ctx, x);
        }
    }

    //--------- Plotting ---------//
    init_window();

    draw_nlp_foam(nlp_ctx, foam_ctx, square_side, 0);

    for (int i = 0; i < sample_count; i++)
    {
        draw_x(nlp_ctx, samples[i]);
        tensor_free(samples[i]);
    }
    printf("Feasible Count: %d; Infeasible Count: %d\n", feasible_count, infeasible_count);

    //--------- Freeing Memory ---------//
    window_wait();
    free_window();

    tensor_free(x);
    nlp_free(nlp_ctx);
    foam_free(foam_ctx);

    return 0;
}

int main()
{
    int failure_count = 0;

    // failure_count += test_nlp_feasibility();
    // failure_count += test_constraint_sampling();
    failure_count += test_foam_constraint_sampling();
    
    if (failure_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failure_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
