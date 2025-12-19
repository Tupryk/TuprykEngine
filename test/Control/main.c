#define DEBUG

#include <stdio.h>
#include "../../TuprykEngine/Control/spring.h"


int main()
{
    struct spring_system_1d s;
    s.pos = 5;
    s.vel = 0;
    s.constant = 20;
    s.friction = 60;
    s.mass = 20;

    float delta_t = 1e-2;
    int steps = 1000;

    float damping_ratio = spring_system_1d_damping_ratio(&s);
    float natural_frequency = spring_system_1d_natural_frequency(&s);
    
    printf("[");
    for (int i = 0; i < steps; i++)
    {
        spring_system_1d_step(&s, delta_t);
        // printf("Position at time %f: %f\n", (i+1)*delta_t, s.pos);
        printf("%f, ", s.pos);
    }
    printf("]\n");

    printf("Damping ratio: %f", damping_ratio);
    if (damping_ratio > 1) printf(" - The system is overdamped!\n");
    else if (damping_ratio < 1) printf(" - The system is underdamped!\n");
    else printf(" - The system is critically damped!\n");
    printf("Natural frequency: %f\n", natural_frequency);

    return 0;
}
