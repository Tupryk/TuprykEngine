#ifndef NEAT_UTILS
#define NEAT_UTILS

#include "agent.h"
#include "population.h"

int evolve_population(population_t* pop, float (*eval_agent)(agent_t*), int generation_count);

#endif
