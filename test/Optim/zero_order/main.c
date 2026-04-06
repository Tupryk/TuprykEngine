// #define DEBUG
#define OPTIM_VERBOSE

#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include "../functions.h"
#include "../../../TuprykEngine/Optim/meta.h"
#include "../../../TuprykEngine/Optim/zero_order/genetic.h"


float task1_eval(float* input, float* predicted_out)
{
    float real_out[2];
    real_out[0] = 0.5 * input[0] + input[1] * -0.1;
    real_out[1] = -0.9 * input[0] + input[2] * 0.3;

    return -1.f * (pow(predicted_out[0] - real_out[0], 2) + pow(predicted_out[1] - real_out[1], 2));
}

int main()
{
    int generation_count = 1000;
    int task_trials = 100;
    int in_dim = 3;
    int out_dim = 2;

    population* pop = init_population(in_dim, out_dim);

    for (int i = 0; i < generation_count; i++)
    {
        float max_score = -1e6;
        float scores[pop->max_size];
        for (int j = 0; j < pop->max_size; j++)
        {
            scores[j] = 0.f;
        }
        for (int j = 0; j < task_trials; j++)
        {
            float input[] = {
                ((float)(rand() % 1000)) * 0.001f * 2.f - 1.f,
                ((float)(rand() % 1000)) * 0.001f * 2.f - 1.f,
                ((float)(rand() % 1000)) * 0.001f * 2.f - 1.f
            };
            float** outputs = population_feed_all_agents(pop, input, i+1==30);
            for (int k = 0; k < pop->max_size; k++)
            {
                scores[k] += task1_eval(input, outputs[k]);
                free(outputs[k]);
            }
            free(outputs);
        }
        for (int k = 0; k < pop->max_size; k++)
        {
            scores[k] /= (float) task_trials;
            if (scores[k] > max_score)
            {
                max_score = scores[k];
            }
            // printf("Score for agent %d: %f\n", k, scores[k]);
        }
        if ((i + 1) % 1 == 0)
        {
            printf("Best score for generation %d: %f\n", i+1, max_score);
        }
        population_kill_weak(pop, scores);
        population_mutate(pop);
    }
    // population_free(pop);
    return 0;
}
