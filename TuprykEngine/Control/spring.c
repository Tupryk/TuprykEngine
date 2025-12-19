#include "spring.h"

#include <math.h>


void spring_system_1d_step(struct spring_system_1d* s, float delta_t)
{
    float acc = -1.f * (s->friction * s->vel + s->constant * s->pos) / s->mass;
    s->vel += acc * delta_t;
    s->pos += s->vel * delta_t;
}

float spring_system_1d_damping_ratio(struct spring_system_1d* s)
{
    return s->friction / ( 2.f * sqrt( s->constant * s->mass ) );
}

float spring_system_1d_natural_frequency(struct spring_system_1d* s)
{
    return sqrt( s->constant / s->mass );
}
