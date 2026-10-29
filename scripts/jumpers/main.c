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
#include "../../TuprykEngine/Algos/utils.h"
#include "../../TuprykEngine/Physics/voxels.h"
#include "../../TuprykEngine/Stochastic/sample.h"
#include "../../TuprykEngine/Physics/voxel_generators.h"

#include "../../TuprykEngine/ui/graphics/ogl.h"
#include "../../TuprykEngine/ui/prints/linalg.h"
#include "../../TuprykEngine/ui/prints/vis_neat.h"
#include "../../TuprykEngine/ui/graphics/voxels_vis.h"

#include "utils.h"

#define FEATURE_DIM 16
int in_dim = FEATURE_DIM*4;
int out_dim = FEATURE_DIM;
int conv_steps = 10;
int agent_idx = 0;
float freq = 5.f;
float voxel_dim = 0.1f;
float tau = 0.0001f;

// Global structs
struct VoxelSim* vs;
population_t* pop;
agent_t* current_agent;
int gen_count = 1;
int pop_size = 30;

void actuation_step()
{
    float time = ((float)vs->t) * vs->tau;
    vs->temp = sinf(time * 2.f * M_PI * freq);
    voxel_step(vs);
}

void render_grid()
{
    for (int i = 0; i < 100; i++) actuation_step();
    render_voxels(vs);
    printf("Simulation time: %f\n", vs->tau * ((float) vs->t));
}

void* sample_viable_agent()
{
    float* v = (float*) malloc(sizeof(float));
    rand_uni(10.f, 100.f);
    return (void*) v;
}

void ann_feed(float* in, float* out)
{
    agent_feed(current_agent, in_dim, out_dim, in, out);
    for (int i = 0; i < out_dim; i++) out[i] = clip(out[i], -1.f, 1.f);
}

float eval_agent(agent_t* agent)
{
    current_agent = agent;
    
    // Setup voxel world
    vs = voxel_init(voxel_dim, 0);
    vs->tau = tau;
    voxel_add_cellular(vs, ann_feed, conv_steps, FEATURE_DIM);
    if (vs->count < 9)
    {
        voxel_free(vs);
        return 0.f;
    }
    
    // Simulate voxels
    float ep_time = 10.f / freq;  // Num. of cycles times cycle frequency
    int total_steps = ep_time / vs->tau;
    float max_z = 0.f;
    
    for (int i = 0; i < total_steps; i++)
    {
        actuation_step();
        for (int j = 0; j < vs->count; j++)
        {
            float z = vs->pos->values[j*3+2];
            if (z > max_z) max_z = z;
        }
    }

    // Compute final score
    float score = max_z;
    if (isinf(score) || score > 100.f)
    {
        printf("Simulator returned NaN!\n");
        score = -1.f;
    }
    voxel_free(vs);
    return score;
}

int main()
{
    srand( time( NULL ) );

    pop = init_population(in_dim, out_dim, pop_size);
    int best_agent_idx = evolve_population(pop, eval_agent, gen_count);
    // int best_agent_idx = 0;
    current_agent = pop->agents[best_agent_idx];

    vs = voxel_init(voxel_dim, 0);
    vs->tau = tau;

    // voxel_add_cellular(vs, human_made_cellular_automaton, 10, FEATURE_DIM);
    voxel_add_cellular(vs, ann_feed, conv_steps, FEATURE_DIM);
    
    printf("Total voxel count: %d\n", vs->count);
    printf("Total shell count: %d/%d\n", vs->shell_count, vs->count);

    // Rendering
    ogl_init();
    ogl_set_speed(voxel_dim * 50.f, 0.7f);
    ogl_set_camera_pos(0.f, voxel_dim * -30.f, 1.f);
    ogl_loop(render_grid);

    // Free memory
    population_free(pop);
    voxel_free(vs);
    return 0;
}
