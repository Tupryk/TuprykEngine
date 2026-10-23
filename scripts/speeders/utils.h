#ifndef SPEEDER_SCRIPT_UTILS
#define SPEEDER_SCRIPT_UTILS

#include "../../TuprykEngine/NEAT/agent.h"

// Network IO
#define IN_DIM 6
#define OUT_DIM 5
extern float nn_input[IN_DIM];


void cppn(float* pos, float* out, agent_t* agent);

#endif
