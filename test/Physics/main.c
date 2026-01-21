#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../TuprykEngine/visual/prints/linalg.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Physics/particle.h"

#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/visual/graphics/basic.h"


static struct ParticleSim* g_ps = NULL;
static int g_point_count = 0;
static int g_dim = 0;

void spin(struct ParticleSim* ps, struct tensor* acc)
{
    for (int j = 0; j < g_point_count; j++)
    {
        acc->values[j * g_dim + 0] = (ps->pos->values[j * g_dim + 1] - 0.5f) *  1.f;
        acc->values[j * g_dim + 1] = (ps->pos->values[j * g_dim + 0] - 0.5f) * -1.f;
    
        // acc->values[j * g_dim + 0] += (ps->pos->values[j * g_dim + 0] - 0.5f) * -10.f;
        // acc->values[j * g_dim + 1] += (ps->pos->values[j * g_dim + 1] - 0.5f) * -10.f;
    }
}

static void draw_ps(void)
{
    particle_sim_step(g_ps, spin);

    draw_3d_unit_cube();

    for (int j = 0; j < g_point_count; j++)
    {
        draw_3d_point(
            g_ps->pos->values[j * g_dim + 0],
            g_ps->pos->values[j * g_dim + 1],
            g_ps->pos->values[j * g_dim + 2]
        );
    }
    add_global_rot(0.0f, 0.01f, 0.0f);
}

int test_particle_sim(void)
{
    g_dim = 3;
    g_point_count = 100;
    
    g_ps = particle_sim_init(g_point_count, g_dim);

    window_wait_with_func(draw_ps);

    particle_sim_free(g_ps);

    g_ps = NULL;

    return 0;
}

int main(void)
{
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
