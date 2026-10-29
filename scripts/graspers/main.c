#define DEBUG

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// #include "../../TuprykEngine/MCP/mcp.h"
#include "../../TuprykEngine/NEAT/agent.h"
#include "../../TuprykEngine/Optim/meta.h"
#include "../../TuprykEngine/Funcs/funcs.h"
#include "../../TuprykEngine/Physics/voxels.h"
#include "../../TuprykEngine/Stochastic/sample.h"
#include "../../TuprykEngine/Physics/voxel_generators.h"

#include "../../TuprykEngine/ui/graphics/ogl.h"
#include "../../TuprykEngine/ui/prints/vis_neat.h"
#include "../../TuprykEngine/ui/graphics/voxels_vis.h"

#include "utils.h"


// Global structs
struct VoxelSim* vs;

void render_grid()
{
    for (int i = 0; i < 100; i++)
    {
        fixed_grasp_motion(vs, 0.f, 2.f, -0.2f);
        voxel_step(vs);
    }
    render_voxels(vs);
    printf("Simulation time: %f\n", vs->tau * ((float) vs->t));
}

void* sample_viable_agent()
{
    float* v = (float*) malloc(sizeof(float));
    rand_uni(10.f, 100.f);
    return (void*) v;
}

int main()
{
    srand( time( NULL ) );

    // 1. Replicate the RSS locomotion paper
    //     - Check if NEAT is working correctly
    //     - Faster simulation -> use const and static or so
    //     - Store and load NEAT populations
    // 2. Formulate MCC for hand/obj co-evolution
    //     - Generate voxel things with L-Systems
    //     - Implement novelty search for initial population?
    //     - First do MC without the box being evolved
    //     - Formulate MCP speciation
    //     - Check if collisions have friction cone? (Up down movement of hand)

    float voxel_dim = 1.f / 20.f;
    vs = voxel_init(voxel_dim, 1);

    // voxel_add_sequence(vs, human_made_sequence_hand0, 4, 1000, 0.f, 0.f, 0.f);
    voxel_add_implicit(vs, human_made_implicit_hand, 20, 0.f, 0.f, 1.1f);
    voxel_add_implicit(vs, human_made_implicit_cube, 5, 0.f, 0.f, 0.35f);
    
    // vs->gravity = 0.f;
    printf("Total voxel count: %d\n", vs->count);
    printf("Total shell count: %d/%d\n", vs->shell_count, vs->count);

    // Rendering
    ogl_init();
    ogl_set_speed(10.0f, 0.7f);
    ogl_set_camera_pos(0.f, -3.f, 1.f);
    ogl_loop(render_grid);

    // Free memory
    voxel_free(vs);
    return 0;
}
