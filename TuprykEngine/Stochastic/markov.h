#ifndef MARKOV
#define MARKOV

#include "../LinAlg/sparse.h"


struct markov_chain
{
    int current_state;
    int state_count;
    int* visited_counts;
    struct sparse* transition_probs;
};

struct markov_chain* new_markov_chain(struct sparse* st, int start_state);
void markov_chain_free(struct markov_chain* mc);
void markov_chain_step(struct markov_chain* mc);
void markov_chain_steps(struct markov_chain* mc, int step_count);

#endif
