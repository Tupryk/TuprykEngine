#include <stdio.h>

#include "utils.h"


int evolve_population(population_t* pop, float (*eval_agent)(agent_t*), int generation_count)
{
    int best_agent_idx = 0;
    float scores[pop->size];

    for (int i = 0; i < generation_count; i++)
    {
        // Evaluate creatures
        for (int j = 0; j < pop->size; j++)
        {
            scores[j] = eval_agent(pop->agents[j]);
            printf("Evaluating member %d of generation %d... Score: %f\n", j+1, i+1, scores[j]);
        }

        // Report generation results
        best_agent_idx = 0;
        float max_score = scores[0];
        for (int j = 1; j < pop->size; j++)
        {
            if (scores[j] > max_score)
            {
                max_score = scores[j];
                best_agent_idx = j;
            }
        }
        printf("Best score for generation %d: %f\n", i+1, max_score);

        // Resample population
        population_resample(pop, scores);
    }

    return best_agent_idx;
}
