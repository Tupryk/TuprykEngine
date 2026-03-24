#ifndef GENETIC
#define GENETIC

#include "../../Algos/lists.h"


typedef struct
{
    int noisy;  // Adds gaussian noise to outputs
    int count;
    int connection_count;
    int** connections;  // Indices of entire network, not just mutation set!
    float* connection_weights;
    int* activation_type;
} new_nodes_gene;

typedef struct
{
    int count;
    int** connections;
    float* connection_weights;
} new_weights_gene;

typedef struct
{
    int start_idx;
    int end_idx;
    float weight;
} new_weight_gene;

typedef struct
{
    int start_idx;
    int connection_idx;
    float new_weight;
} weight_perturvation_gene;

// TODO: Add a noise as input mechanism to the network, ie. like in diffusion models
// TODO: Add dropout somehow
// TODO: Adaptime uniman schedule that only moves when the population is ready (above a certain success threshhold)

typedef struct
{
    int node_count;
    int* new_activation;
    float* bias_perturvation;
} node_perturvation_gene;

typedef struct
{
    int weigth_count;
    float* weight_perturvations;
} weights_perturvation_gene;

typedef struct
{
    int type;
    void* data;
} gene;

typedef struct
{
    int gene_count;
    int node_count;
    int* genes;
    
    // TODO: this should be part of a different struct (network struct) maybe...
    float* activations;
    int* activation_count;
    int* activation_funcs;

    int* connection_counts;
    int** connections;
    float** connection_weight;
} agent;

typedef struct
{
    int in_dim;
    int out_dim;
    int max_size;
    int current_size;
    int keep_best_n;
    int agent_children_count;
    vector innovations;  // Type: gene
    agent** agents;
} population;

population* init_population(int in_dim, int out_dim);
void population_mutate(population* pop);
void population_kill_weak(population* pop, float* scores);
float* feed_agent(agent* a, float* input, int in_dim, int out_dim);
float** population_feed_all_agents(population* pop, float* input);
void population_free(population* pop);
void agent_free(agent* a);
void gene_free(gene g);

#endif
