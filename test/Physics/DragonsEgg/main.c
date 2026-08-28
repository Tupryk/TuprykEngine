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
int g_point_count = 1000;
int g_max_point_count = 1000;
tensor_t* cam_pos = NULL;

void follow_colors(struct ParticleSim* ps, tensor_t* acc)
{
    float dir[3];
    float dist_vec[3];

    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;
        for (int j = 0; j < ps->max_count; j++)
        {
            if (ps->energy->values[j] <= 0.f) continue;
            if (i == j) continue;

            float* p1 = &ps->pos->values[i*3];
            float* p2 = &ps->pos->values[j*3];

            dist_vec[0] = p2[0] - p1[0];
            dist_vec[1] = p2[1] - p1[1];
            dist_vec[2] = p2[2] - p1[2];

            float dist = sqrtf(
                dist_vec[0] * dist_vec[0] +
                dist_vec[1] * dist_vec[1] +
                dist_vec[2] * dist_vec[2]
            );

            if (dist < 15.f && fabsf(dist) > 0.001f)
            {
                dir[0] = dist_vec[0] / dist;
                dir[1] = dist_vec[1] / dist;
                dir[2] = dist_vec[2] / dist;

                float* v1 = &ps->vel->values[i*3];
    
                float* c1 = &ps->color->values[i*3];
                float* c2 = &ps->color->values[j*3];
    
                // float attraction = (c1[0] * c2[0] + c1[1] * c2[1] + c1[2] * c2[2]) * 10.f;

                float attraction = 0.f;
                if (c1[0] > 0.75f && c2[2] < 0.75f) attraction = 1.f;
                if (c1[0] < 0.75f && c2[2] > 0.75f) attraction = -1.f;

                if (c1[2] > 0.75f && c2[1] < 0.75f) attraction = 1.f;
                if (c1[2] < 0.75f && c2[1] > 0.75f) attraction = -1.f;

                if (c1[1] > 0.75f && c2[0] < 0.75f) attraction = 1.f;
                if (c1[1] < 0.75f && c2[0] > 0.75f) attraction = -1.f;

                attraction *= 10.f;
    
                v1[0] += attraction * dir[0];
                v1[1] += attraction * dir[1];
                v1[2] += attraction * dir[2];
            }
        }
    }
}

void ps_loop()
{
    particle_sim_euler_step(g_ps, follow_colors);
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
    float cam_pos_values[] = {0.f, -200.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count, g_max_point_count);
    // g_ps->pos->values[0] = -2.f;
    // g_ps->pos->values[1] = 0.f;
    // g_ps->pos->values[2] = 0.f;
    // g_ps->pos->values[3] = 2.f;
    // g_ps->pos->values[4] = 0.f;
    // g_ps->pos->values[5] = 0.f;
    
    // g_ps->vel->values[0] = 1.f;
    // g_ps->vel->values[3] = -1.f;

    // g_ps->sizes->values[0] = 1.5f;

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
