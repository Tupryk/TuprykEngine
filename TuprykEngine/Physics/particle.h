#ifndef PARTICLE_SIM
#define PARTICLE_SIM

#include "../LinAlg/tensor.h"
#include "../LinAlg/sparse.h"


struct ParticleSim
{
    int t;
    int count;
    int max_count;
    float tau;
    float max_vel;
    
    tensor_t* pos;
    tensor_t* vel;
    tensor_t* color;
    tensor_t* sizes;
    tensor_t* energy;
    tensor_t* age;
    sparse_t* links;
};

struct ParticleSim* particle_sim_init(int init_particle_count, int max_particle_count);
void particle_sim_free(struct ParticleSim* ps);
void particle_sim_euler_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, tensor_t*));
void particle_sim_resolve_collisions(struct ParticleSim* ps);
void particle_sim_cap_vels(struct ParticleSim* ps);
void particle_sim_wrap_pos(struct ParticleSim* ps, float bounds);
void particle_sim_update_energy(struct ParticleSim* ps);
void particle_sim_duplicate_particles(struct ParticleSim* ps);

#endif
