#define DEBUG

#include <time.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../TuprykEngine/Physics/atom.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Stochastic/sample.h"

#include "../../../TuprykEngine/ui/graphics/ogl.h"
#include "../../../TuprykEngine/ui/prints/linalg.h"
#include "../../../TuprykEngine/ui/graphics/atom_sim.h"


struct AtomSim* g_ps = NULL;
int g_point_count = 1500;
tensor_t* cam_pos = NULL;

void print_timestep()
{
    printf("Time %d (%gs)\n", g_ps->t, ((float)g_ps->t) * g_ps->tau);
}

void none(struct AtomSim* ps, tensor_t* acc) { }

float cos_sim(float* a, float* b, int dim)
{
    float norm_a = 0.f;
    float norm_b = 0.f;
    float dot = 0.f;

    for (int i = 0; i < dim; i++)
    {
        norm_a += a[i] * a[i];
        norm_b += b[i] * b[i];
        dot += a[i] * b[i];
    }

    norm_a = sqrtf(norm_a);
    norm_b = sqrtf(norm_b);

    return dot / (norm_a * norm_b);
}

void follow_features(struct AtomSim* ps, tensor_t* acc)
{
    float dir[3];
    float dist_vec[3];
    int feature_dim = ps->features->shape[1];
    float min_dist = 5.f;
    float max_dist = 20.f;

    for (int i = 0; i < ps->count; i++)
    {
        for (int j = i+1; j < ps->count; j++)
        {
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

            if (dist < max_dist)
            {
                //////////////////////////
                if (dist < min_dist*1.1)
                {
                    ogl_draw_line(
                        p1[0], p1[1], p1[2],
                        p2[0], p2[1], p2[2],
                        1.f, 1.f, 1.f, 0.5f
                    );
                }
                //////////////////////////

                if (dist < 1e-3)
                {
                    dist = 0.01f;
                    dist_vec[0] = rand_uni(-1.f, 1.f) * dist;
                    dist_vec[1] = rand_uni(-1.f, 1.f) * dist;
                    dist_vec[2] = rand_uni(-1.f, 1.f) * dist;
                }

                dir[0] = dist_vec[0] / dist;
                dir[1] = dist_vec[1] / dist;
                dir[2] = dist_vec[2] / dist;

                float* v1 = &ps->vel->values[i*3];
                float* v2 = &ps->vel->values[j*3];
    
                float* c1 = &ps->features->values[i * feature_dim];
                float* c2 = &ps->features->values[j * feature_dim];
    
                float attraction = cos_sim(c1, c2, feature_dim);

                float pull = (dist - min_dist) / (max_dist - min_dist);
                if (pull > 0.f) pull *= attraction;
                else pull *= 10.f; // TODO: Is this correct?
                
                v1[0] += pull * dir[0];
                v1[1] += pull * dir[1];
                v1[2] += pull * dir[2];

                v2[0] -= pull * dir[0];
                v2[1] -= pull * dir[1];
                v2[2] -= pull * dir[2];
            }
        }
    }
}

void ps_step_render()
{
    atoms_step(g_ps, follow_features);
    render_ps(g_ps);
    if (g_ps->t % 10 == 0)
    {
        print_timestep();
    }
}

int test_atoms()
{
    g_ps = atoms_init(g_point_count);

    ogl_init();

    for (int i = 0; i < 100; i++)
    {
        atoms_step(g_ps, follow_features);
        if (g_ps->t % 100 == 0) print_timestep();
    }
    
    // Rendering
    // ogl_init();

    ogl_set_speed(50.0f, 0.3f);
    ogl_set_camera_pos(0.f, -200.f, 0.f);

    ogl_loop(ps_step_render);
    
    atoms_free(g_ps);
    return 0;
}

int main()
{
    srand( time( NULL ) );

    int failures_count = 0;
    failures_count += test_atoms();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    
    return 0;
}
