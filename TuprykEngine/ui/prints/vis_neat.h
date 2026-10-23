#ifndef VIS_NEAT
#define VIS_NEAT

#include "../../NEAT/agent.h"
#include "../../NEAT/population.h"


static const char *activation_names[] = {
    "Linear", "ReLU", "Leaky ReLU", "Sigmoid", "sin", "Gaussian"
};

void print_innovations(vector_t* innovations);
void print_innovation(vector_t* innovations, size_t i);
void print_agents(population_t* pop);
void print_agent(agent_t* a);
void print_species_info(population_t* pop);
void print_agent_gene_sequence(vector_t* innovations, agent_t* a);
void print_agent_genes(vector_t* innovations, agent_t* a);
void print_population_best_agent(population_t* pop, float* scores);
void print_activation_name(int act_id);

#endif
