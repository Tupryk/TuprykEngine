#define DEBUG

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../TuprykEngine/ui/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Physics/particle.h"

#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/particle_sim.h"


struct ParticleSim* g_ps = NULL;
int g_point_count = 100;
int g_max_point_count = 500;
tensor_t* cam_pos = NULL;

int show_every = 10000;
int show_for = 500;

void print_timestep()
{
    printf("Time %d (%gs) - Particle count: %d/%d\n", g_ps->t, ((float)g_ps->t) * g_ps->tau, g_ps->count, g_ps->max_count);
}

void ps_step_render()
{
    particle_sim_step(g_ps);
    render_ps(g_ps, cam_pos);
    if (g_ps->t % 10 == 0)
    {
        print_timestep();
    }
}

int test_particle_sim()
{
    float cam_pos_values[] = {0.f, -200.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count, g_max_point_count);

    while (1)
    {
        // Render current state
        init_window();
        window_wait_steps_with_func(ps_step_render, show_for, 16);
        free_window();
        
        // Step sim without rendering
        for (int i = show_for; i < show_every; i++)
        {
            particle_sim_step(g_ps);
            if (g_ps->t % 1000 == 0) print_timestep();
        }
    }

    particle_sim_free(g_ps);
    tensor_free(cam_pos);
    return 0;
}

int main()
{
    srand( time( NULL ) );

    int failures_count = 0;
    failures_count += test_particle_sim();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    
    return 0;
}
