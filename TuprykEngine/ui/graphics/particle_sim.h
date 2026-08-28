#ifndef PARTICLE_SIM_RENDER
#define PARTICLE_SIM_RENDER

#include "../../LinAlg/tensor.h"
#include "../../Physics/particle.h"

void render_ps(struct ParticleSim* ps, tensor_t* cam_pos);

#endif
