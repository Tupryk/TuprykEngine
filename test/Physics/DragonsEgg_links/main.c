#define DEBUG

#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../TuprykEngine/ui/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Physics/particle.h"

#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/particle_sim.h"


struct ParticleSim* g_ps = NULL;
int g_point_count = 2;
int g_max_point_count = 2;
tensor_t* cam_pos = NULL;

void none(struct ParticleSim* ps, tensor_t* acc) { }

void ps_loop()
{
    particle_sim_euler_step(g_ps, none);
    particle_sim_resolve_collisions(g_ps);
    particle_sim_cap_vels(g_ps);
    particle_sim_update_energy(g_ps);
    particle_sim_duplicate_particles(g_ps);
    particle_sim_wrap_pos(g_ps, 64.f);

    render_ps(g_ps, cam_pos);
    
    printf("(Time %d) - Particle count: %d/%d\n", g_ps->t, g_ps->count, g_ps->max_count);
}

int test_particle_sim()
{
    float cam_pos_values[] = {0.f, -20.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count, g_max_point_count);

    g_ps->pos->values[0] = -2.f;
    g_ps->pos->values[1] = 0.f;
    g_ps->pos->values[2] = 0.f;
    g_ps->pos->values[3] = 2.f;
    g_ps->pos->values[4] = 0.f;
    g_ps->pos->values[5] = 0.f;

    g_ps->color->values[0] = 0.f;
    g_ps->color->values[1] = 1.f;
    g_ps->color->values[2] = 0.f;
    g_ps->color->values[3] = 1.f;
    g_ps->color->values[4] = 0.f;
    g_ps->color->values[5] = 0.f;
    
    g_ps->vel->values[0] = 1.f;
    g_ps->vel->values[3] = -1.f;

    g_ps->sizes->values[0] = 1.5f;
    g_ps->sizes->values[1] = 1.f;

    window_wait_with_func(ps_loop);

    particle_sim_free(g_ps);
    tensor_free(cam_pos);
    return 0;
}

int main()
{
    srand( time( NULL ) );

    init_window();

    int failures_count = 0;
    failures_count += test_particle_sim();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    free_window();
    return 0;
}
