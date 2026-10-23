#ifndef NEAT_AGENT
#define NEAT_AGENT

#include <math.h>
#include "../Funcs/funcs.h"


#define ACTIVATION_COUNT 6
static float (*activations[ACTIVATION_COUNT])(float) = {
    linear, relu, leaky_relu, sigmoid, sinf, gaussian
};

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

    int* out_count;
    int** out_id;
    float** out_weight;

    int* in_count;
} agent_t;

agent_t* agent_new(int initial_weight_count);
void agent_free_network(agent_t* a);
void agent_free(agent_t* a);

void agent_feed(agent_t* a, int in_dim, int out_dim, float* input, float* output);
float agent_compatibility_dist(agent_t* agent_a, agent_t* agent_b);
agent_t* agents_cross(agent_t* parent_a, agent_t* parent_b);
agent_t* agent_copy_genome(agent_t* a);

#endif
