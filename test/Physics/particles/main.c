#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/ui/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Physics/particle.h"

#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/particle_sim.h"


struct ParticleSim* g_ps = NULL;
int g_point_count = 100;
tensor_t* cam_pos = NULL;

void spin(struct ParticleSim* ps, tensor_t* acc)
{
    for (int j = 0; j < g_point_count; j++)
    {
        acc->values[j * 3 + 0] = (ps->pos->values[j * 3 + 1] - 0.5f) *  1.f;
        acc->values[j * 3 + 1] = (ps->pos->values[j * 3 + 0] - 0.5f) * -1.f;
    }
}

void ps_loop()
{
    particle_sim_euler_step(g_ps, spin);
    render_ps(g_ps, cam_pos);
}

int test_particle_sim()
{
    float cam_pos_values[] = {0.f, -100.f, 0.f};
    cam_pos = new_tensor_vector(3, cam_pos_values);

    g_ps = particle_sim_init(g_point_count);

    window_wait_with_func(ps_loop);

    particle_sim_free(g_ps);
    tensor_free(cam_pos);
    return 0;
}

int main()
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
