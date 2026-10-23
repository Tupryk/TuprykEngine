/// Unshackling Evolution: Evolving Soft Robots with Multiple
/// Materials and a Powerful Generative Encoding (2013)
/// Nick Cheney, Robert MacCurdy, Jeff Clune, Hod Lipson

#define DEBUG

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../TuprykEngine/NEAT/agent.h"
#include "../../TuprykEngine/Optim/meta.h"
#include "../../TuprykEngine/Funcs/funcs.h"
#include "../../TuprykEngine/Physics/voxels.h"
#include "../../TuprykEngine/NEAT/population.h"
#include "../../TuprykEngine/Stochastic/sample.h"

#include "../../TuprykEngine/ui/graphics/ogl.h"
#include "../../TuprykEngine/ui/prints/vis_neat.h"
#include "../../TuprykEngine/ui/graphics/voxels_vis.h"

#include "utils.h"


// Evaluation
int generation_count = 1000;  // 1000
int best_agent_idx = 0;
int pop_size = 30;  // 30
int resolution = 8;  // 10
float freq = 50.f;
float amplitud = 1.f;

// Global structs
struct VoxelSim* vs;
population_t* pop;
int agent_idx = 0;


float compute_displacement()
{
    float center[2] = {0.f, 0.f};
    for (int i = 0; i < vs->count; i++)
    {
        center[0] += vs->pos->values[0];
        center[1] += vs->pos->values[1];
    }
    center[0] /= (float) vs->count; center[1] /= (float) vs->count;
    
    return sqrtf(center[0]*center[0] + center[1]*center[1]);
}

void actuation_step()
{
    float time = ((float)vs->t) * vs->tau;
    vs->temp = sinf((time / (2.f * M_PI)) * freq) * amplitud;
    // if (vs->temp > 0.f) vs->temp = 0.f;
    voxel_step(vs);
}

void render_grid()
{
    for (int i = 0; i < 100; i++) actuation_step();

    // Compute final score
    float score = compute_displacement();

    printf("Simulation time: %f (Episode time: %f)\n", vs->tau * ((float) vs->t), 10.f / freq);
    printf("Score: %f\n", score);
    render_voxels(vs);
}

void implicit_func(float* pos, float* out) { cppn(pos, out, pop->agents[agent_idx]); }

float eval_agent(int id)
{
    clock_t total_time = clock();

    agent_idx = id;
    
    // Setup voxel world
    clock_t init_time = clock();

    float voxel_dim = 1.f / ((float) resolution);
    vs = voxel_init(voxel_dim, 0);
    float z_offset = -vs->voxel_dim * ((float)resolution) * 0.5f;
    voxel_add_implicit(vs, implicit_func, resolution, 0.f, 0.f, z_offset);
    if (vs->count < 20)
    {
        voxel_free(vs);
        return 0.f;
    }
    
    init_time = clock() - init_time;

    float init_disp = compute_displacement();

    // Simulate voxels
    clock_t sim_time = clock();

    float ep_time = 10.f / freq;  // Num. of cycles times cycle frequency
    int total_steps = ep_time / vs->tau;
    for (int i = 0; i < total_steps; i++)
    {
        actuation_step();
    }
    
    sim_time = clock() - sim_time;

    float final_disp = compute_displacement();
    
    // Compute final score
    float score = fabsf(final_disp - init_disp);
    if (isnan(score))
    {
        printf("Simulator returned NaN!\n");
        score = -1.f;
    }
    voxel_free(vs);

    // total_time = clock() - total_time;
    // printf(
    //     "Total time: %.3f ms (%.3f % Init Sim, %.3f % Run Sim)\n",
    //     (((float)total_time) / CLOCKS_PER_SEC) * 1000.f,
    //     ((float) init_time) / ((float)total_time) * 100.f,
    //     ((float) sim_time) / ((float)total_time) * 100.f
    // );

    return score;
}

void evolve_population(population_t* pop)
{
    float scores[pop->size];

    for (int i = 0; i < generation_count; i++)
    {
        // Evaluate creatures
        for (int j = 0; j < pop->size; j++)
        {
            scores[j] = eval_agent(j);
            // printf("Evaluating member %d of generation %d... Score: %f\n", j+1, i+1, scores[j]);
        }

        // Report generation results
        best_agent_idx = 0;
        float max_score = scores[0];
        for (int j = 1; j < pop->size; j++)
        {
            if (scores[j] > max_score)
            {
                max_score = scores[j];
                best_agent_idx = j;
            }
        }
        printf("Best score for generation %d: %f\n", i+1, max_score);

        // Resample population
        population_resample(pop, scores);
    }
}

int main()
{
    srand( time( NULL ) );

    // Setup implicit function
    printf("Initializing population with dimensions; in: %d, out: %d\n", IN_DIM, OUT_DIM);
    pop = init_population(IN_DIM, OUT_DIM, pop_size);
    evolve_population(pop);

    // Evaluating best
    agent_idx = best_agent_idx;
    print_agent(pop->agents[agent_idx]);
    
    float voxel_dim = 1.f / ((float) resolution);
    vs = voxel_init(voxel_dim, 0);
    float z_offset = -vs->voxel_dim * ((float)resolution) * 0.5f;
    voxel_add_implicit(vs, implicit_func, resolution, 0.f, 0.f, z_offset);
    
    // Rendering
    ogl_init();
    ogl_set_speed(10.0f, 0.7f);
    ogl_set_camera_pos(0.f, -3.f, 0.f);
    ogl_loop(render_grid);

    // Free memory
    population_free(pop);
    voxel_free(vs);
    return 0;
}
