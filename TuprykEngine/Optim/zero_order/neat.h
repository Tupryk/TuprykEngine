#ifndef NEAT
#define NEAT

#include "../../global.h"
#include "../../Algos/lists.h"


typedef struct
{
    int in;
    int out;
} gene_t;

typedef struct
{
    int gene_count;
    int node_count;
    int weight_count;
    int* genes;
    int* gene_enabled;
    
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

    int max_size;
    int keep_best_n;
    int agent_children_count;
    
    float network_size_cost_weight;
    
    int node_counter;
    
    vector innovations;  // Type: gene
    agent_t** agents;
} population_t;

population_t* init_population(int in_dim, int out_dim);
void population_mutate(population_t* pop);
void population_kill_weak(population_t* pop, float* scores);
float* feed_agent(agent_t* a, float* input, int in_dim, int out_dim);
float** population_feed_all_agents(population_t* pop, float* input);
void population_free(population_t* pop);
void build_agent_network(population_t* pop, agent_t* a);
void free_agent_network(agent_t* a);
void agent_free(agent_t* a);
void gene_free(gene_t g);

#endif
