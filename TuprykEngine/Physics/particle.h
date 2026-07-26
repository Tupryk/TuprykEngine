#ifndef PARTICLE_SIM
#define PARTICLE_SIM

#include "../LinAlg/tensor.h"


struct ParticleSim
{
    int dim;
    float t;
    float tau;
    float rk4_h;
    
    tensor_t* pos;
    tensor_t* vel;

    tensor_t* action_space;
};

struct ParticleSim* particle_sim_init(int particle_count, int sim_dim);
void particle_sim_free(struct ParticleSim* ps);
void particle_sim_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, tensor_t*));

#endif
