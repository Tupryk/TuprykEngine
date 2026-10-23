#ifndef HANDS_SCRIPT_UTILS
#define HANDS_SCRIPT_UTILS

#include "../../TuprykEngine/NEAT/agent.h"

// Network IO
static const int in_dim = 6;
static const int out_dim = 5;
static float nn_input[in_dim];


void cppn(float* pos, float* out, agent_t* agent);

#endif
