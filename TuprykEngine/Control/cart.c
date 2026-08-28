#include <math.h>

#include "cart.h"
#include "../LinAlg/tensor.h"


void cart_vec(struct cart_config_t* C, tensor_t* state, tensor_t* out)
{
    float x_dot = state[1];
    float theta = state[2];
    float theta_dot = state[3];

    float M = C->c_mass;
    float m = C->p_mass;
    float l = C->p_length_to_com;
    float g = C->g;

    float u = 0.f;

    float s = sinf(theta);
    float c = cosf(theta);

    float denom = M + m - m * c*c;

    float x_ddot = (u + m * s * (l * theta_dot * theta_dot + g * c)) / denom;

    float theta_ddot = (
        -u * c
        - m * l * theta_dot * theta_dot * c * s
        - (M + m) * g * s
    ) / (l * denom);

    out[0] = x_dot;
    out[1] = x_ddot;
    out[2] = theta_dot;
    out[3] = theta_ddot;
}

void cart_step(struct cart_config_t* C, tensor_t* state, float dt)
{
    // RK4
    float state_[] = {
        state->x,
        state->x_dot,
        state->theta,
        state->theta_dot
    };
    
    float k1[4];
    cart_vec(C, state_, k1);

    float k2[4];
    cart_vec(C, state_, k1);
    
    float k3[4];
    cart_vec(C, state_, k1);
    
    float k4[4];
    cart_vec(C, state_, k1);

    k1 = f(t, u)
    k2 = f(t + dt / 2, u + dt / 2 * k1)
    k3 = f(t + dt / 2, u + dt / 2 * k2)
    k4 = f(t + dt, u + dt * k3)

    t_next = t + dt
    u_next = u + dt / 6 * (k1 + 2 * k2 + 2 * k3 + k4)

    tensor_sequence("t*2.0 + (t-10.0+v)", X, A, tmp);
    state->x += k1[0] * dt;
    state->x_dot += k1[1] * dt;
    state->theta += k1[2] * dt;
    state->theta_dot += k1[3] * dt;
}
