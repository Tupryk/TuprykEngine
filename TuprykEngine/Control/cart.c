#include <math.h>

#include "cart.h"


void cart_step(struct cart_config* C, struct cart_state* state, float dt)
{
    float M = C->c_mass;
    float m = C->p_mass;
    float l = C->p_length_to_com;
    float g = C->g;

    float u = 0.f;

    float s = sinf(state->theta);
    float c = cosf(state->theta);

    float denom = M + m - m * c*c;

    float x_ddot = (u + m * s * (l * state->theta_dot*state->theta_dot + g * c)) / denom;

    float theta_ddot = (
        -u * c
        - m * l * state->theta_dot*state->theta_dot * c * s
        - (M + m) * g * s
    ) / (l * denom);


    state->x += state->x_dot * dt;
    state->x_dot += x_ddot * dt;
    state->theta += state->theta_dot * dt;
    state->theta_dot += theta_ddot * dt;
}
