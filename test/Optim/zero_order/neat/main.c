// #define DEBUG
#define OPTIM_VERBOSE

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../functions.h"
#include "../../../../TuprykEngine/Optim/meta.h"
#include "../../../../TuprykEngine/Stochastic/sample.h"
#include "../../../../TuprykEngine/Optim/zero_order/neat.h"

#include "../../../../TuprykEngine/visual/prints/vis_neat.h"
#include "../../../../TuprykEngine/visual/graphics/genetic.h"


float task1_eval(float* input, float* predicted_out)
{
    float real_out[2];
    real_out[0] = 0.5 * input[0] + input[1] * -0.1;
    real_out[1] = -0.9 * input[0] + input[2] * 0.3;

    return -1.f * (pow(predicted_out[0] - real_out[0], 2) + pow(predicted_out[1] - real_out[1], 2));
}

float task2_eval(float* input, float* predicted_out)
{
    float real_out[2];
    real_out[0] = sin(0.5 * input[0]) + input[1] * -0.1;
    real_out[1] = -2.f * cos(input[2] * 0.3);

    return -1.f * (pow(predicted_out[0] - real_out[0], 2) + pow(predicted_out[1] - real_out[1], 2));
}

int test_init()
{
    int in_dim = 3;
    int out_dim = 2;

    population_t* pop = init_population(in_dim, out_dim);
    printf("Initial agents:\n");
    print_agents(pop);

    print_agent_genes(&pop->innovations, pop->agents[0]);
    draw_network(pop, pop->agents[0]);
    
    population_free(pop);
    return 0;
}

int test_speciation()
{
    int in_dim = 3;
    int out_dim = 2;

    population_t* pop = init_population(in_dim, out_dim);

    for (int i = 0; i < pop->size; i++)
    {
        printf("Agent %d species: %d\n", i, pop->agent_to_species[i]);
    }
    print_species_info(pop);

    printf("Agent A:\n");
    print_agent_gene_sequence(&pop->innovations, pop->agents[0]);
    printf("Agent B:\n");
    print_agent_gene_sequence(&pop->innovations, pop->agents[1]);

    float comp_dist = agent_compatibility_dist(pop->agents[0], pop->agents[1]);
    printf("Compatibility distance: %g\n", comp_dist);
    
    population_free(pop);
    return 0;
}

int test_crossover()
{
    int in_dim = 3;
    int out_dim = 2;

    population_t* pop = init_population(in_dim, out_dim);

    printf("Parent A:\n");
    print_agent_gene_sequence(&pop->innovations, pop->agents[0]);
    printf("Parent B:\n");
    print_agent_gene_sequence(&pop->innovations, pop->agents[1]);

    agent_t* child = agents_cross(pop->agents[0], pop->agents[1]);
    printf("Child:\n");
    print_agent_gene_sequence(&pop->innovations, child);
    
    agent_free(child);
    population_free(pop);
    return 0;
}

int test_feed_agent()
{
    int in_dim = 3;
    int out_dim = 2;
    float input[3] = {1.f, 0.f, 0.f};
    float output[2];

    population_t* pop = init_population(in_dim, out_dim);

    int agent_idx = 0;
    print_agent(pop->agents[agent_idx]);
    printf("Feeding agent %d: \n", agent_idx);

    agent_feed(pop->agents[agent_idx], in_dim, out_dim, input, output);

    printf("Given input: [");
    for (int i = 0; i < pop->in_dim; i++)
    {
        printf("%g, ", input[i]);
    }
    printf("]\nAgent outputs: [");
    for (int i = 0; i < pop->out_dim; i++)
    {
        printf("%g, ", output[i]);
    }
    printf("]\n");

    population_free(pop);
    return 0;
}

int test_resample()
{
    int in_dim = 3;
    int out_dim = 2;
    int generation_count = 1000;

    population_t* pop = init_population(in_dim, out_dim);
    printf("Initial agents:\n");
    print_agents(pop);

    for (int i = 0; i < generation_count; i++)
    {
        float fitness[pop->size];
        for (int j = 0; j < pop->size; j++)
        {
            fitness[j] = rand_uni(0.f, 10.f);
        }
    
        population_resample(pop, fitness);
    
        if ((i+1) % 100 == 0)
        {
            printf("Generation %d species count: %zu\n", i+1, pop->species.size);
        }
    }

    population_free(pop);
    return 0;
}

int test_full()
{
    int in_dim = 3;
    int out_dim = 2;
    int generation_count = 1000;
    int task_trials = 100;

    population_t* pop = init_population(in_dim, out_dim);
    
    float scores[pop->size];
    float** output = (float**) malloc(sizeof(float*) * pop->size);
    for (int i = 0; i < pop->size; i++) output[i] = (float*) malloc(sizeof(float) * out_dim);

    for (int i = 0; i < generation_count; i++)
    {
        float max_score = -1e6;
        memset(scores, 0.f, sizeof(scores));
        for (int j = 0; j < task_trials; j++)
        {
            float input[] = {rand_uni(0.f, 1.f), rand_uni(0.f, 1.f), rand_uni(0.f, 1.f)};
            population_feed_all_agents_same(pop, input, output);
            for (int k = 0; k < pop->size; k++)
            {
                scores[k] += task1_eval(input, output[k]);
            }
        }
        for (int k = 0; k < pop->size; k++)
        {
            scores[k] /= (float) task_trials;
            if (scores[k] > max_score)
            {
                max_score = scores[k];
            }
        }
        if ((i + 1) % 1 == 0)
        {
            printf("Best score for generation %d: %f\n", i+1, max_score);
        }
        population_resample(pop, scores);
    }
    print_population_best_agent(pop, scores);
    population_free(pop);

    for (int i = 0; i < pop->size; i++) free(output[i]);
    free(output);
    return 0;
}

int main()
{
    srand( time( NULL ) );

    int failure_count = 0;

    // failure_count += test_init();
    // failure_count += test_speciation();
    // failure_count += test_crossover();
    // failure_count += test_feed_agent();
    // failure_count += test_resample();
    failure_count += test_full();
    
    if (failure_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failure_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
