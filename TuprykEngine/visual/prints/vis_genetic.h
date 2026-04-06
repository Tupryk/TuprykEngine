#ifndef VIS_GENETIC
#define VIS_GENETIC

#include "../../Optim/zero_order/genetic.h"

void print_innovations(vector* innovations);
void print_innovation(vector* innovations, size_t i);
void print_agents(agent** agents, int count);
void print_agent(agent* a);
void print_agent_genes(vector* innovations, agent* a);

#endif
