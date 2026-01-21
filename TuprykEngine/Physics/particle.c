#include "particle.h"

#include <stdlib.h>
#include <stdio.h>

#include "../LinAlg/tensor.h"


struct ParticleSim* particle_sim_init(int particle_count, int sim_dim)
{
    #ifdef DEBUG
    if (sim_dim <= 0)
    {
        fprintf(stderr, "Error: invalid simulation dimension\n");
        exit(EXIT_FAILURE);
    }
    #endif
    struct ParticleSim* ps = (struct ParticleSim*) malloc(sizeof(struct ParticleSim));
    
    ps->t = 0.f;
    ps->tau = 0.005f;
    ps->rk4_h = 0.02f;
    ps->dim = sim_dim;
    
    int shape[1] = {particle_count * sim_dim};
    ps->pos = new_tensor(shape, 1, NULL);
    ps->vel = new_tensor(shape, 1, NULL);

    int action_space_shape[2] = {sim_dim, sim_dim};
    ps->action_space = new_tensor(action_space_shape, 2, NULL);
    for (int i = sim_dim; i < sim_dim * 2; i++)
    {
        ps->action_space->values[i] = 1.f;
    }

    for (int i = 0; i < ps->pos->shape[0]; i++)
    {
        ps->pos->values[i] = rand() % 1000 * 0.001f;
    }

    return ps;
}

void particle_sim_free(struct ParticleSim* ps)
{
    tensor_free(ps->pos);
    tensor_free(ps->vel);
    free(ps);
}

// void rk4_step(struct tensor* x, vector_field dydt)
// {
//     k1 = f(t, x)
//     k2 = f(t + h * 0.5f, x + h * k1 * 0.5f)
//     k3 = f(t + h * 0.5f, x + h * k2 * 0.5f)
//     k4 = f(t + h, x + h * k3)

//     x += (h/6) * (k1 + 2.f * k2 + 2.f * k3 + k4)
// }

void particle_sim_step(struct ParticleSim* ps, void (*dydt)(struct ParticleSim*, struct tensor*))
{
    struct tensor* tmp = tensor_copy_shape(ps->pos);

    // Position update
    tensor_scalar_mult(ps->vel, ps->tau, tmp);
    tensor_add(ps->pos, tmp, ps->pos);

    // Velocity update
    dydt(ps, tmp);
    tensor_scalar_mult(tmp, ps->tau, tmp);
    tensor_add(ps->vel, tmp, ps->vel);

    ps->t += ps->tau;

    tensor_free(tmp);
}
