#define DEBUG

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Physics/particle.h"
#include "../../../TuprykEngine/ui/prints/linalg.h"

#include "../../../TuprykEngine/ui/graphics/ogl.h"
#include "../../../TuprykEngine/ui/graphics/particle_sim.h"


struct ParticleSim* g_ps = NULL;
int g_point_count = 100;
int g_max_point_count = 1000;
tensor_t* cam_pos = NULL;

int show_every = 10000;
int show_for = 250;

void print_cell_code(int i)
{
    if (g_ps->energy->values[i] <= 0.f)
    {
        printf("( Dead Cell )\n");
    }
    
    int8_t* memory = g_ps->code_memory[i];

    int current_line = 0;

    while (1)
    {
        int next_line = current_line + 1;
    
        int command = memory[current_line] % g_ps->command_count;
        switch (command)
        {
            case 0: break; // printf("%d: no-op", current_line); break;
            case 1: // goto (where)
            {
                if (current_line + 1 >= g_ps->memory_size) break;
                int8_t where_id = memory[current_line + 1];
                printf("%d: goto %d\n", current_line, where_id);
                next_line = current_line + 2;
                break;
            }
            case 2: // write (what, where)
            {
                if (current_line + 2 >= g_ps->memory_size) break;
                int8_t what_id = memory[current_line + 1];
                int8_t where_id = memory[current_line + 2];
                printf("%d: write %d %d\n", current_line, what_id, where_id);
                next_line = current_line + 3;
                break;
            }
            case 3: // write links (what, where)
            {
                if (current_line + 2 >= g_ps->memory_size) break;
                int8_t what_id = memory[current_line+1];
                int8_t where_id = memory[current_line+2];
                printf("%d: write_links %d %d\n", current_line, what_id, where_id);
                next_line = current_line + 3;
                break;
            }
            case 4: // sleep (current_time, wait_time)
            {
                if (current_line + 2 >= g_ps->memory_size) break;
                int8_t current_time_id = memory[current_line+1];
                int8_t wait_time_id = memory[current_line+2];
                printf("%d: sleep %d %d\n", current_line, current_time_id, wait_time_id);
                next_line = current_line + 3;
                break;
            }
            case 5: printf("%d: contract\n", current_line); break;
            case 6: printf("%d: open_switch\n", current_line); break;
            case 7: printf("%d: duplicate_switch\n", current_line); break;
            default: printf("%d: undefined (%d)\n", current_line, memory[current_line]); break;
        }
        if (next_line >= g_ps->memory_size) break;
        current_line = next_line;
    }
}

void print_timestep()
{
    // print_cell_code(0);
    printf("----- Time %d (%gs) -----\n", g_ps->t, ((float)g_ps->t) * g_ps->tau);
    printf("- Particle count: %d/%d\n", g_ps->count, g_ps->max_count);
    printf("- Total links: %zu\n", g_ps->link_data->size);
}

void ps_step_render()
{
    particle_sim_step(g_ps);
    render_ps(g_ps);
    if (g_ps->t % 10 == 0)
    {
        print_timestep();
    }
}

int test_particle_sim()
{
    g_ps = particle_sim_init(g_point_count, g_max_point_count);

    for (int i = 0; i < 2000; i++)
    {
        particle_sim_step(g_ps);
        if (g_ps->t % 100 == 0) print_timestep();
    }

    // Rendering
    ogl_init();

    ogl_set_speed(50.0f, 0.3f);
    ogl_set_camera_pos(0.f, -200.f, 0.f);

    ogl_loop(ps_step_render);

    particle_sim_free(g_ps);
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
