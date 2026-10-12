#include "atom.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>


void atoms_clip_vel(struct AtomSim* ps)
{
    for (int i = 0; i < ps->count; i++)
    {
        float* vel = &ps->vel->values[i*3];
        float vel_norm = sqrtf(vel[0] * vel[0] + vel[1] * vel[1] + vel[2] * vel[2]);
        
        if (vel_norm > ps->max_vel)
        {
            vel[0] *= ps->max_vel / vel_norm;
            vel[1] *= ps->max_vel / vel_norm;
            vel[2] *= ps->max_vel / vel_norm;
        }
    }
}

void atoms_clip_pos(struct AtomSim* ps, float bounds)
{
    // TODO: Reflect velocity with respect to the normal like in the regular collisions
    float* pos = ps->pos->values;
    float* vel = ps->vel->values;

    for (int i = 0; i < ps->count * 3; i++)
    {
        if (pos[i] > bounds)
        {
            pos[i] = bounds;
            if (vel[i] > 0.f) vel[i] *= -0.5f;
        }
        if (pos[i] < -bounds)
        {
            pos[i] = -bounds;
            if (vel[i] < 0.f) vel[i] *= -0.5f;
        }
    }
}
