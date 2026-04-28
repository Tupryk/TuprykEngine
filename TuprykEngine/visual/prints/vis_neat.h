#ifndef VIS_NEAT
#define VIS_NEAT

#include "../../Optim/zero_order/neat.h"

void print_innovations(vector* innovations);
void print_innovation(vector* innovations, size_t i);
void print_agents(agent_t** agents, int count);
void print_agent(agent_t* a);
void print_agent_gene_sequence(vector* innovations, agent_t* a);
void print_agent_genes(vector* innovations, agent_t* a);
void print_population_best_agent(population_t* pop, float* scores);
void print_activation_name(int act_id);

#endif
