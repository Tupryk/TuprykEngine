#ifndef NEAT
#define NEAT

#include "../../global.h"
#include "../../Algos/lists.h"


// TODO: Reject species that do not improve over n generations.
// TODO: Interspecies mating rate.
// TODO: Checking if the mutation already exists should be done generation wide and not full history wise.

typedef struct
{
    int in;
    int out;
} gene_t;

typedef struct
{
    int gene_count;
    int node_count;
    
    int* genes;
    int* gene_enabled;
    float* gene_weights;
    
    float* activations;
    int* activation_count;
    int* activation_funcs;
    float* activation_biases;

    int* connection_counts;
    int** connections;
    float** connection_weight;
} agent_t;

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
    
    vector innovations;  // Type: gene_t
    agent_t** agents;

    int* agent_to_species;
    vector species;  // Type: agent_t*
} population_t;

population_t* init_population(int in_dim, int out_dim);
void population_feed_all_agents_same(population_t* pop, float* input, float** output);
void population_feed_all_agents(population_t* pop, float** input, float** output);
void population_resample(population_t* pop, float* fitness);
void population_free(population_t* pop);

void agent_feed(agent_t* a, int in_dim, int out_dim, float* input, float* output);
void agent_maybe_mutate(population_t* pop, agent_t* a);
void agent_build_network(population_t* pop, agent_t* a);
void agent_free_network(agent_t* a);
void agent_free(agent_t* a);
float agent_compatibility_dist(agent_t* agent_a, agent_t* agent_b);
agent_t* agents_cross(agent_t* parent_a, agent_t* parent_b);

#endif
