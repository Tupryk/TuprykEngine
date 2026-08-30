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
int g_point_count = 3;
int g_max_point_count = 10;
tensor_t* cam_pos = NULL;

void none(struct ParticleSim* ps, tensor_t* acc) { }

void ps_loop()
{
    particle_sim_euler_step(g_ps, none);
    particle_sim_resolve_collisions(g_ps);
    particle_sim_cap_vels(g_ps);
    // particle_sim_update_energy(g_ps);
    particle_sim_duplicate_particles(g_ps);
    particle_sim_wrap_pos(g_ps, 64.f);
    particle_sim_run_genes(g_ps);

    render_ps(g_ps, cam_pos);
    
    printf("(Time %d) - Particle count: %d/%d\n", g_ps->t, g_ps->count, g_ps->max_count);
}

int test_particle_sim()
{
    float cam_pos_values[] = {0.f, -20.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count, g_max_point_count);

    // TEST ORGANISM
    organism_t* organism = (organism_t*) malloc(sizeof(organism_t));
    organism->particle_ids = int_stack_init();
    int_stack_push(organism->particle_ids, 0);
    int_stack_push(organism->particle_ids, 1);
    int_stack_push(organism->particle_ids, 2);

    organism->links = stack_init();
    link_t* link_a = new_link(
        1, 0, 1.f, 1.f,
        -1.f, 0.f, 0.f
    );
    stack_push(organism->links, link_a);
    link_t* link_b = new_link(
        1, 2, 1.f, 1.f,
        1.f, 0.f, 0.f
    );
    stack_push(organism->links, link_b);

    organism->com = new_tensor_vector(3, NULL);
    organism->rot = new_tensor_vector(4, NULL);
    organism->vel = new_tensor_vector(3, NULL);
    organism->ang_vel = new_tensor_vector(3, NULL);

    stack_push(g_ps->organisms, organism);

    // PARTICLE SPECIFICATIONS
    tensor_fill(g_ps->pos, 0.f);
    g_ps->pos->values[0] = -1.f;
    g_ps->pos->values[6] = 1.f;

    tensor_fill(g_ps->color, 0.f);
    g_ps->color->values[0] = 1.f;
    g_ps->color->values[3] = 1.f;
    g_ps->color->values[6] = 1.f;
    
    g_ps->sizes->values[0] = 1.f;
    g_ps->sizes->values[1] = 1.f;
    g_ps->sizes->values[2] = 1.f;

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
