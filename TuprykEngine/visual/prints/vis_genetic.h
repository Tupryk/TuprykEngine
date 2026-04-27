#ifndef VIS_GENETIC
#define VIS_GENETIC

#include "../../Optim/zero_order/genetic.h"

void print_innovations(vector* innovations);
void print_innovation(vector* innovations, size_t i);
void print_agents(agent_t** agents, int count);
void print_agent(agent_t* a);
void print_agent_genes(vector* innovations, agent_t* a);
void print_population_best_agent(population_t* pop, float* scores);
void print_activation_name(int act_id);

#endif
