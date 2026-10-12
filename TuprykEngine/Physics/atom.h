#ifndef ATOM_SIM
#define ATOM_SIM

#include "../Algos/sds.h"
#include "../LinAlg/tensor.h"


struct AtomSim
{
    int t;
    int count;
    
    float tau;
    float max_vel;

    tensor_t* pos;
    tensor_t* vel;
    tensor_t* features;

    struct grid_sds* grid;
};

struct AtomSim* atoms_init(int particle_count);
void atoms_free(struct AtomSim* ps);
void atoms_step(struct AtomSim* ps, void (*dydt)(struct AtomSim*, tensor_t*));

#endif
