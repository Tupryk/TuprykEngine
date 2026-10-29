/// Code based on:
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

#include "../../TuprykEngine/NEAT/utils.h"
#include "../../TuprykEngine/NEAT/agent.h"
#include "../../TuprykEngine/Optim/meta.h"
#include "../../TuprykEngine/Funcs/funcs.h"
#include "../../TuprykEngine/Physics/voxels.h"
#include "../../TuprykEngine/NEAT/population.h"
#include "../../TuprykEngine/Stochastic/sample.h"
#include "../../TuprykEngine/Physics/voxel_generators.h"

#include "../../TuprykEngine/ui/graphics/ogl.h"
#include "../../TuprykEngine/ui/prints/vis_neat.h"
#include "../../TuprykEngine/ui/graphics/voxels_vis.h"

#include "utils.h"


// Evaluation
int generation_count = 1;  // 1000
int best_agent_idx = 0;
int pop_size = 10;  // 30
int resolution = 4;  // 10
float freq = 4.f;
float amplitud = 1.f;
float voxel_dim = 0.5f;

// Global structs
struct VoxelSim* vs;
population_t* pop;
agent_t* current_agent;


float compute_z_offset()
{
    return vs->voxel_dim * ((float)resolution) * 0.5f + vs->floor_z;
}

float vec_len(float* center)
{
    return sqrtf(center[0]*center[0] + center[1]*center[1]);
}

void compute_center(float* center)
{
    center[0] = 0.f; center[1] = 0.f;
    for (int i = 0; i < vs->count; i++)
    {
        center[0] += vs->pos->values[0];
        center[1] += vs->pos->values[1];
    }
    center[0] /= (float) vs->count; center[1] /= (float) vs->count;
}

void actuation_step()
{
    float time = ((float)vs->t) * vs->tau;
    vs->temp = sinf(time * 2.f * M_PI * freq) * amplitud;  // Probably wrong!!!
    // if (vs->temp > 0.f) vs->temp = 0.f;
    voxel_step(vs);
}

void render_grid()
{
    for (int i = 0; i < 100; i++) actuation_step();

    // Compute final score
    float center[2];
    compute_center(center);
    float score = vec_len(center);

    printf("Simulation time: %f (Episode time: %f)\n", vs->tau * ((float) vs->t), 10.f / freq);
    printf("Score: %f\n", score);
    render_voxels(vs);
}

void implicit_func(float* pos, float* out) { cppn(pos, out, current_agent); }

float eval_agent(agent_t* agent)
{
    clock_t total_time = clock();

    current_agent = agent;
    
    // Setup voxel world
    clock_t init_time = clock();

    vs = voxel_init(voxel_dim, 0);
    float z_offset = vs->voxel_dim * ((float)resolution) * 0.5f - vs->floor_z + vs->voxel_dim;
    voxel_add_implicit(vs, implicit_func, resolution, 0.f, 0.f, z_offset);
    if (vs->count < 20)
    {
        voxel_free(vs);
        return 0.f;
    }
    
    init_time = clock() - init_time;

    // Simulate voxels
    clock_t sim_time = clock();

    float ep_time = 10.f / freq;  // Num. of cycles times cycle frequency
    int total_steps = ep_time / vs->tau;
    float center_start[2];
    float center_end[2];
    
    for (int i = 0; i < total_steps; i++) actuation_step();
    compute_center(center_start);
    
    for (int i = 0; i < total_steps; i++) actuation_step();
    compute_center(center_end);
    
    center_end[0] -= center_start[0];
    center_end[1] -= center_start[1];
    
    sim_time = clock() - sim_time;

    // Compute final score
    float score = vec_len(center_end);
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

int main()
{
    srand( time( NULL ) );

    // Setup implicit function
    printf("Initializing population with dimensions; in: %d, out: %d\n", IN_DIM, OUT_DIM);
    pop = init_population(IN_DIM, OUT_DIM, pop_size);
    best_agent_idx = evolve_population(pop, eval_agent, generation_count);

    // Evaluating best
    current_agent = pop->agents[best_agent_idx];
    print_agent(pop->agents[best_agent_idx]);
    
    vs = voxel_init(voxel_dim, 0);
    float z_offset = compute_z_offset();
    voxel_add_implicit(vs, implicit_func, resolution, 0.f, 0.f, z_offset);
    
    // Rendering
    ogl_init();
    ogl_set_speed(10.0f, 0.7f);
    ogl_set_camera_pos(0.f, -vs->voxel_dim * ((float)resolution) * 5.f, 1.f);
    ogl_loop(render_grid);

    // Free memory
    population_free(pop);
    voxel_free(vs);
    return 0;
}
