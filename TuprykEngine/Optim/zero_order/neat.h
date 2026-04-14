#ifndef NEAT
#define NEAT

#include "../../global.h"
#include "../../Algos/lists.h"


typedef struct
{
    int in;
    int out;
} gene;

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
} agent;

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
    agent** agents;
} population;

population* init_population(int in_dim, int out_dim);
void population_mutate(population* pop);
void population_kill_weak(population* pop, float* scores);
float* feed_agent(agent* a, float* input, int in_dim, int out_dim);
float** population_feed_all_agents(population* pop, float* input);
void population_free(population* pop);
void build_agent_network(population* pop, agent* a);
void free_agent_network(agent* a);
void agent_free(agent* a);
void gene_free(gene g);

#endif
