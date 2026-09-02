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
    
    float phi;
    float theta;
    
    // Spherical coordinates to 3D relative position:
    // x = p * sin(phi) * cos(theta)
    // y = p * sin(phi) * sin(theta)
    // z = p * cos(phi)
    // p = r1 + r2

    // 3D relative position to Spherical coordinates:
    // phi = cos^-1(z / p)
    // theta = cos^-1(x / (p * sin(phi)))

    struct stack_elem* from_elem;
    struct stack_elem* to_elem;
    struct stack_elem* data_elem;

} link_t;

struct ParticleSim
{
    int t;
    int count;
    int max_count;
    
    float tau;
    float max_vel;
    float max_age;
    
    // char** genes;
    int* code_state;  // Current line being read in the genetic code
    int* code_inbox;

    pstack_t** links;
    pstack_t* link_data;
    
    tensor_t* pos;
    tensor_t* vel;
    
    tensor_t* color;
    tensor_t* sizes;
    
    tensor_t* energy;
    tensor_t* age;

    tensor_t* last_read;
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
void particle_sim_resolve_links(struct ParticleSim* ps);
void particle_sim_distribute_energy(struct ParticleSim* ps);

#endif
