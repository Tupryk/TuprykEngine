#define DEBUG
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
    real_out[0] = sin(input[0]*input[1]) + input[1] * input[2] - input[1];
    real_out[1] = input[0] * input[2] - input[1];

    return -1.f * (pow(predicted_out[0] - real_out[0], 2) + pow(predicted_out[1] - real_out[1], 2));
}

int main()
{
    int generation_count = 100;
    int task_trials = 10;

    struct population* pop = init_population(3, 2);

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
            float input[3] = {
                ((float)(rand() % 1000)) * 0.001f * 10.f - 5.f,
                ((float)(rand() % 1000)) * 0.001f * 10.f - 5.f,
                ((float)(rand() % 1000)) * 0.001f * 10.f - 5.f
            };
            float** outputs = population_feed_all_agents(pop, input);
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
        }
        printf("Best score for generation %d: %f\n", i, max_score);
        population_kill_weak(pop, scores);
        population_mutate(pop);
    }
    // population_free(pop);
    return 0;
}
