#ifndef NEAT_POPULATION
#define NEAT_POPULATION

#include "agent.h"
#include "../global.h"
#include "../Algos/lists.h"


// TODO: Reject species that do not improve over n generations.
// TODO: Interspecies mating rate.
// TODO: Checking if the mutation already exists should be done generation wise and not full history wise.
// TODO: Option to allow or reject loops.
// TODO: Do we keep the champion? If not this should be an option.

typedef struct
{
    int in;
    int out;
} gene_t;

typedef struct
{
    int in_dim;
    int out_dim;

    int size;
    float network_size_cost_weight;
    float new_node_mutation_prob;
    float new_link_mutation_prob;
    float weights_mutation_prob;
    float weights_mutation_prob_resample;
    float speciation_thresh;
    float crossover_offspring_prob;
    
    vector_t innovations;  // Type: gene_t
    agent_t** agents;

    int* agent_to_species;
    vector_t species;  // Type: agent_t*
} population_t;

population_t* init_population(int in_dim, int out_dim, int size);
void population_free(population_t* pop);
void population_feed_all_agents_same(population_t* pop, float* input, float** output);
void population_feed_all_agents(population_t* pop, float** input, float** output);
void population_resample(population_t* pop, float* fitness);

void agent_build_network(population_t* pop, agent_t* a);
void agent_maybe_mutate(population_t* pop, agent_t* a);

#endif
