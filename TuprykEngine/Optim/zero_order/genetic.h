#ifndef GENETIC
#define GENETIC


struct new_nodes_gene
{
    int count;
    int connection_count;
    int** connections;  // Indices of entire network, not just mutation set!
    float* connection_weights;
    int* activation_type;
};

struct new_weights_gene
{
    int count;
    int** connections;
    float* connection_weights;
};

// TODO: Add a noise as input mechanism to the network, ie. like in diffusion models
// TODO: Add dropout somehow
// TODO: Adaptime uniman schedule that only moves when the population is ready (above a certain success threshhold)

struct node_perturvation_gene
{
    int node_count;
    int* new_activation;
    float* bias_perturvation;
};

struct weight_perturvation_gene
{
    int weigth_count;
    float* weight_perturvations;
};

struct gene
{
    int type;
    void* data;
    struct gene* next;
};

struct agent
{
    int gene_count;
    int node_count;
    struct gene** genes;
    
    float* activations;
    int* activation_count;
    int* activation_funcs;

    int* connection_counts;
    int** connections;
    float** connection_weight;
};

struct population
{
    int in_dim;
    int out_dim;
    int max_size;
    int current_size;
    int keep_best_n;
    int innovation_count;
    int agent_children_count;
    struct gene* innovations;
    struct gene* latest_innovation;
    struct agent** agents;
};

struct population* init_population(int in_dim, int out_dim);
void population_mutate(struct population* pop);
void population_kill_weak(struct population* pop, float* scores);
float* feed_agent(struct agent* a, float* input, int in_dim, int out_dim);
float** population_feed_all_agents(struct population* pop, float* input);
void population_free(struct population* pop);
void agent_free(struct agent* a);
void gene_free(struct gene* g);

#endif
