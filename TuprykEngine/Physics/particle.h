#ifndef PARTICLE_SIM
#define PARTICLE_SIM

#include "../Algos/lists.h"
#include "../LinAlg/tensor.h"
#include "../LinAlg/sparse.h"


typedef struct
{
    int from;
    int to;
    float strength;
    float damping;
    tensor_t* rel_pos;
} link_t;

typedef struct
{
    int_stack_t* particle_ids;

    pstack_t* links;

    tensor_t* com;
    tensor_t* rot;

    tensor_t* vel;
    tensor_t* ang_vel;

} organism_t;

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

    pstack_t* organisms;
};

struct ParticleSim* particle_sim_init(int init_particle_count, int max_particle_count);
void particle_sim_free(struct ParticleSim* ps);
void particle_sim_euler_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, tensor_t*));
void particle_sim_resolve_collisions(struct ParticleSim* ps);
void particle_sim_cap_vels(struct ParticleSim* ps);
void particle_sim_wrap_pos(struct ParticleSim* ps, float bounds);
void particle_sim_update_energy(struct ParticleSim* ps);
void particle_sim_duplicate_particles(struct ParticleSim* ps);
void particle_sim_run_genes(struct ParticleSim* ps);
link_t* new_link(int from, int to, float strength, float damping, float rel_x, float rel_y, float rel_z);

#endif
