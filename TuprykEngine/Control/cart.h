#ifndef CART
#define CART

struct cart_config
{
    float c_mass;
    float p_mass;
    float p_length_to_com;
    float g;
};

struct cart_state
{
    float x;
    float x_dot;
    float theta;
    float theta_dot;
};

void cart_step(struct cart_config* C, struct cart_state* state, float dt);

#endif
