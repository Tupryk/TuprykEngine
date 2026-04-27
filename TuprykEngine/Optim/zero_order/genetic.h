#ifndef GENETIC
#define GENETIC

#include "../../global.h"
#include "../../Algos/lists.h"


typedef struct
{
    int noisy;  // Adds gaussian noise to outputs
    int count;
    int connection_count;
    int** connections;  // Indices of entire network, not just mutation set!
    float* connection_weights;
    int* activation_type;
    float* activation_biases;
} new_nodes_gene;

typedef struct
{
    int weight_start_idx;
    int weight_connection_idx;
    int activation_type;  // 0: Linear, 1: ReLU, 2: LeakyReLU, 3: Sigmoid
    float bias;
} new_node_gene;

typedef struct
{
    int neuron_id;
    int activation_type;
    float bias;
} node_perturvation_gene;

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
    float weight_perturvation;
} weight_perturvation_gene;

// TODO: Add a noise as input mechanism to the network, ie. like in diffusion models
// TODO: Add dropout somehow
// TODO: Adaptime uniman schedule that only moves when the population is ready (above a certain success threshhold)
// TODO: Evolve a reward model
// TODO: Can agents be sepparated into a diferent file?

typedef struct
{
    int node_count;
    int* new_activation;
    float* bias_perturvation;
} nodes_perturvation_gene;

typedef struct
{
    int weigth_count;
    float* weight_perturvations;
} weights_perturvation_gene;

typedef struct
{
    int type;  // 0: 
    void* data;
} gene;

typedef struct
{
    int gene_count;
    int node_count;
    int weight_count;
    int* genes;
    
    // TODO: this should be part of a different struct (network struct) maybe...
    float* activations;
    int* activation_count;
    int* activation_funcs;
    float* activation_biases;

    int* connection_counts;
    int** connections;
    float** connection_weight;
} agent;

typedef struct
{
    int in_dim;
    int out_dim;
    int max_size;
    int keep_best_n;
    int agent_children_count;
    float network_size_cost_weight;
    vector innovations;  // Type: gene
    agent_t** agents;
} population;

population_t* init_population(int in_dim, int out_dim);
void population_mutate(population_t* pop);
void population_kill_weak(population_t* pop, float* scores);
float* feed_agent(agent_t* a, float* input, int in_dim, int out_dim);
float** population_feed_all_agents(population_t* pop, float* input);
void population_free(population_t* pop);
void build_agent_network(population_t* pop, agent_t* a);
void free_agent_network(agent_t* a);
void agent_free(agent_t* a);
void gene_free(gene g);

#endif
