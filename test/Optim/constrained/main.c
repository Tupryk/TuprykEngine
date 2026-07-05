#define DEBUG
// #define OPTIM_VERBOSE

#include <stdio.h>
#include "../functions.h"
#include "../../../TuprykEngine/Optim/meta.h"
#include "../../../TuprykEngine/Optim/constrained/nlp.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/nhr.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/mcmc.h"
#include "../../../TuprykEngine/Optim/constrained/sampling/masem.h"
#include "../../../TuprykEngine/Optim/constrained/augmented_lagrangian.h"

#include "../../../TuprykEngine/ui/prints/linalg.h"
#include "../../../TuprykEngine/ui/graphics/basic.h"
#include "../../../TuprykEngine/ui/graphics/window.h"


void draw_x(nlp_t* nlp, tensor* x)
{
    int feasible = nlp_feasible(nlp, x);

    if (feasible) set_color(0.1216f, 0.4667f, 0.7059f);
    else set_color(1.0f, 0.4980f, 0.0549f);

    int cx = ((x->values[0] + 1.f) * 0.5f) * ((float) WINDOW_W);
    int cy = ((-x->values[1] + 1.f) * 0.5f) * ((float) WINDOW_H);
    draw_circle(cx, cy, 2);
}

void draw_c(tensor* x)
{
    set_color(0.1216f, 0.4667f, 0.7059f);
    int cx = ((x->values[0] + 1.f) * 0.5f) * ((float) WINDOW_W);
    int cy = ((-x->values[1] + 1.f) * 0.5f) * ((float) WINDOW_H);
    draw_circle(cx, cy, 2);
}

int test_aug_lagrangian()
{
    float values[] = {11.f, -5.f};
    tensor* init_x = new_tensor_vector(2, values);
    
    quadratic_init();
    nlp_t* nlp = get_nlp0();

    printf("--- Test 1: Augmented Lagrangian ---\n");
    aug_lagrangian_init(nlp, 1e-1, 1e-2, 10, 10, 1);
    aug_lagrangian_run(init_x);
    aug_lagrangian_free();
    
    nlp_free(nlp);
    tensor_free(init_x);
    quadratic_free();

    return 0;
}

int test_nlp_feasibility()
{
    int sample_count = 1000;
    tensor* x = new_tensor_vector(2, NULL);
    
    nlp_t* nlp = get_nlp_swiss_roll();

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    printf("--- Test 2: NLP Feasibility ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        tensor_fill_uniform(x, -1.f, 1.f);
        draw_x(nlp, x);
    }
    window_wait();
    free_window();
    
    nlp_free(nlp);
    tensor_free(x);

    return 0;
}

int test_constraint_sampling()
{
    int sample_count = 1000;
    tensor* x = new_tensor_vector(2, NULL);
    
    nlp_t* nlp = get_nlp_swiss_roll();

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    printf("--- Test 3: Constraint Sampling ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        tensor_fill_uniform(x, -1.f, 1.f);
        
        aug_lagrangian_init(nlp, 1e-2, 1e-2, 10, 10, 1);
        
        struct nlp_optim_logs* col = aug_lagrangian_run(x);
        tensor_transfer_values(x, col->final_x);
        
        aug_lagrangian_free();
        
        draw_x(nlp, x);
    }
    window_wait();
    free_window();
    
    nlp_free(nlp);
    tensor_free(x);

    return 0;
}

int test_nhr_constraint_sampling()
{
    int sample_count = 1000;
    
    nlp_t* nlp = get_nlp_swiss_roll();
    tensor* samples[sample_count];
    tensor* feasible_point = new_tensor_vector(2, NULL);
    
    nhr_sample(nlp, feasible_point, sample_count, 1.f, samples);
    
    tensor_free(feasible_point);

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    printf("--- Test 4: NHR Constraint Sampling ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        draw_x(nlp, samples[i]);
        tensor_free(samples[i]);
    }
    window_wait();
    free_window();
    
    nlp_free(nlp);

    return 0;
}

int test_mcmc_sampling()
{
    int sample_count = 1000;
    tensor* x = new_tensor_vector(2, NULL);
    
    tensor* samples[sample_count];
    mcmc_sample(gaussian, x, 0.01f, sample_count, samples);
    
    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();

    printf("--- Test 5: MCMC Sampling ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        draw_c(samples[i]);
        tensor_free(samples[i]);
    }
    window_wait();
    free_window();
    
    tensor_free(x);

    return 0;
}

int test_masem_constraint_sampling()
{
    int sample_count = 1000;
    
    nlp_t* nlp = get_nlp_swiss_roll();
    tensor* samples[sample_count];
    tensor* feasible_point = new_tensor_vector(2, NULL);
    
    masem_sample(nlp, sample_count, 2.f, 1, 10, 5, samples);
    
    tensor_free(feasible_point);

    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();
    
    printf("--- Test 6: MASEM Constraint Sampling ---\n");
    for (int i = 0; i < sample_count; i++)
    {
        draw_x(nlp, samples[i]);
        tensor_free(samples[i]);
    }
    window_wait();
    free_window();
    
    nlp_free(nlp);

    return 0;
}

int main()
{
    int failure_count = 0;

    // failure_count += test_aug_lagrangian();
    // failure_count += test_nlp_feasibility();
    failure_count += test_constraint_sampling();
    // failure_count += test_nhr_constraint_sampling();
    // failure_count += test_mcmc_sampling();
    // failure_count += test_masem_constraint_sampling();
    
    if (failure_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failure_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
