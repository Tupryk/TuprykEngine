#ifndef CONTROL_SPRING
#define CONTROL_SPRING

struct spring_system_1d
{
    float pos;
    float vel;

    float constant;
    float friction;
    float mass;
};

void spring_system_1d_step(struct spring_system_1d* s, float delta_t);
float spring_system_1d_damping_ratio(struct spring_system_1d* s);
float spring_system_1d_natural_frequency(struct spring_system_1d* s);

#endif
