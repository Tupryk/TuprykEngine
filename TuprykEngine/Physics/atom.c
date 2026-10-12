#include "atom.h"
#include "atom_utils.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../LinAlg/tensor.h"
#include "../Stochastic/sample.h"


struct AtomSim* atoms_init(int particle_count)
{
    struct AtomSim* ps = (struct AtomSim*) malloc(sizeof(struct AtomSim));
    
    ps->t = 0;
    ps->count = particle_count;
    
    ps->tau = 0.01f;
    ps->max_vel = 100.f;

    ps->pos = new_tensor_matrix(particle_count, 3, NULL);
    ps->vel = new_tensor_matrix(particle_count, 3, NULL);
    ps->features = new_tensor_matrix(particle_count, 3, NULL);

    tensor_fill_uniform(ps->pos, -50.f, 50.f);
    tensor_fill(ps->vel, 0.f);
    tensor_fill_uniform(ps->features, -1.f, 1.f);

    ps->grid = grid_sds_init(9, 40.f);

    return ps;
}

void atoms_free(struct AtomSim* ps)
{
    tensor_free(ps->pos);
    tensor_free(ps->vel);
    tensor_free(ps->features);
    grid_sds_free(ps->grid);
    free(ps);
}

void atoms_step(struct AtomSim* ps, void (*dydt)(struct AtomSim*, tensor_t*))
{
    ps->t++;
    tensor_t* tmp = tensor_copy_shape(ps->pos);

    // Position update
    tensor_scalar_mult(ps->vel, ps->tau, tmp);
    tensor_add(ps->pos, tmp, ps->pos);

    tensor_scalar_mult(ps->vel, 0.5f, ps->vel);

    // Velocity update
    dydt(ps, tmp);
    tensor_scalar_mult(tmp, ps->tau, tmp);
    tensor_add(ps->vel, tmp, ps->vel);
    
    tensor_free(tmp);

    // Clip velocities
    atoms_clip_vel(ps);

    // Clip pos
    // atoms_clip_pos(ps, 50.f);
}
