#ifndef FEATURES
#define FEATURES

#include "../LinAlg/tensor.h"
#include "../LinAlg/gtensor.h"
#include "../Kinematics/configuration.h"

void p2p_vec(config_t* C, int parent_id, tensor_t* rel_pos, tensor_t* target, gtensor_t* out);

#endif
