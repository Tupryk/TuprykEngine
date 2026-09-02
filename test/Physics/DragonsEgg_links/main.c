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
int g_point_count = 10;
int g_max_point_count = 10;
tensor_t* cam_pos = NULL;

link_t* new_link(int from, int to, float x, float y, float z)
{
    link_t* link = (link_t*) malloc(sizeof(link_t));
    link->from = from;
    link->to = to;
    link->strength = 1.f;
    link->damping = 1.f;
    float p = sqrt(x*x + y*y + z*z);
    link->phi = acos(z / p);
    link->theta = atan2(y, x);
    return link;
}

void none(struct ParticleSim* ps, tensor_t* acc) { }

void ps_loop()
{
    particle_sim_euler_step(g_ps, none);
    particle_sim_resolve_collisions(g_ps);
    particle_sim_cap_vels(g_ps);
    particle_sim_update_energy(g_ps);
    particle_sim_duplicate_particles(g_ps);
    particle_sim_wrap_pos(g_ps, 64.f);
    particle_sim_run_genes(g_ps);
    particle_sim_resolve_links(g_ps);
    particle_sim_distribute_energy(g_ps);

    render_ps(g_ps, cam_pos);
    
    printf("Time %d (%gs) - Particle count: %d/%d\n", g_ps->t, ((float)g_ps->t) * g_ps->tau, g_ps->count, g_ps->max_count);
}

int test_particle_sim()
{
    float cam_pos_values[] = {0.f, -20.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count, g_max_point_count);
    tensor_fill(g_ps->pos, 0.0f);
    tensor_fill(g_ps->color, 0.0f);
    tensor_fill(g_ps->energy, 0.2f);
    tensor_fill(g_ps->age, 0.f);
    g_ps->energy->values[0] = 1.f;
    g_ps->code_state[0] = 1;

    // Creating an organism manually
    // RING
    tensor_fill(g_ps->sizes, 0.5f);
    for (int i = 0; i < g_point_count; i++)
    {
        int i3 = i*3;
        g_ps->color->values[i3] = 1.f;
        float ang = ((float)i) / ((float)g_point_count) * M_PI * 2.f;
        g_ps->pos->values[i3] = cosf(ang);
        g_ps->pos->values[i3 + 2] = -sinf(ang);
        
        int from = i==0 ? g_point_count-1 : i-1;
        int to = i;
        
        link_t* link = new_link(
            from, to, -1.f, 0.f, 0.f
        );
        link->from_elem = stack_push(g_ps->links[from], link);
        link->to_elem = stack_push(g_ps->links[to], link);
        link->data_elem = stack_push(g_ps->link_data, link);
    }

    // FISH
    // tensor_fill(g_ps->sizes, 0.5f);
    // tensor_fill(g_ps->color, 1.0f);
    // for (int i = 0; i < g_point_count; i++)
    // {
    //     int i3 = i * 3;
    //     g_ps->color->values[i3] = 1.f;
    //     g_ps->pos->values[i3] = ((float) i) - 5.f;
    //     g_ps->pos->values[i3+2] = ((float) (i % 3));
        
    //     int from = i==0 ? g_point_count-1 : i-1;
    //     int to = i;
        
    //     link_t* link = new_link(
    //         from, to, -1.f, 0.f, 0.f
    //     );
    //     vector_push(&g_ps->links[from], &link);
    //     vector_push(&g_ps->links[to], &link);
    //     stack_push(g_ps->link_data, link);
        
    //     if ((i+2) % 3)
    //     {
    //         g_ps->color->values[i3+1] = 0.f;
    //         g_ps->color->values[i3+2] = 0.f;
    //     }
    // }

    window_wait_with_func(ps_loop);
    // while (1) ps_loop();

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
