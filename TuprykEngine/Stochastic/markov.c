#include "markov.h"

#include <stdio.h>
#include <stdlib.h>


struct markov_chain* new_markov_chain(struct sparse* st, int start_state)
{
    struct markov_chain* mc = (struct markov_chain*) malloc(sizeof(struct markov_chain));
    
    mc->transition_probs = st;
    mc->state_count = st->shape[0];

    mc->current_state = start_state;

    #ifdef DEBUG
    if (st->shape_dim != 2 || st->shape[0] != st->shape[1])
    {
        printf("Transition probability matrix must be square!\n");
        exit(EXIT_FAILURE);
    }
    if (mc->current_state >= mc->state_count)
    {
        printf("Cannot start at state %d with only %d only states!\n", mc->current_state, mc->state_count);
        exit(EXIT_FAILURE);
    }
    // TODO:
    // int c = 0;
    // int r = 0;
    // float sum = 0.f;
    // for (int i = 0; i < st->value_count; i++)
    // {
    //     sum += 
    //     if (st->values[i] < 0.f || st->values[i] > 1.f)
    //     {
    //         printf("Probabilities must be between one and zero!\n");
    //         exit(EXIT_FAILURE);
    //     }
    //     if (sum != 0.f && abs(sum - 1.f) > 1e-5)
    //     {
    //         printf("Row probabilities need to add up to either one or zero!\n");
    //         exit(EXIT_FAILURE);
    //     }
    // }
    #endif

    mc->visited_counts = (int*) malloc( sizeof(int) * mc->state_count );

    for (int i = 0; i < mc->state_count; i++)
    {
        mc->visited_counts[i] = 0;
    }
    mc->visited_counts[mc->current_state]++;

    return mc;
}

void markov_chain_free(struct markov_chain* mc)
{
    sparse_free(mc->transition_probs);
    free(mc->visited_counts);
    free(mc);
}

void markov_chain_step(struct markov_chain* mc)
{
    float picked = ((float)(rand() % (int)1e6)) * 1e-6;

    const int tpvc = mc->transition_probs->value_count;
    int* tpis = mc->transition_probs->indices;
    float sum = 0.f;

    // TODO: This might not be optimal in some cases...
    for (int i = 0; i < tpvc; i++)
    {
        if (tpis[i] % mc->state_count == mc->current_state)
        {
            sum += mc->transition_probs->values[i];
            if (picked < sum && sum != 0.f)
            {
                mc->current_state = tpis[i] / mc->state_count;
                break;
            }
        }
    }

    mc->visited_counts[mc->current_state]++;
}

void markov_chain_steps(struct markov_chain* mc, int step_count)
{
    for (int i = 0; i < step_count; i++)
    {
        markov_chain_step(mc);
    }
}
