#define DEBUG

#include <math.h>
#include <float.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../TuprykEngine/MCP/mcp.h"
#include "../../TuprykEngine/ui/prints/algos.h"
#include "../../TuprykEngine/Stochastic/sample.h"


void* sample_viable_agent()
{
    float* v = (float*) malloc(sizeof(float));
    v[0] = rand_uni(10.f, 100.f);
    return (void*) v;
}

void* resample_agent(void* agent)
{
    float* v = (float*) agent;
    float* c = (float*) malloc(sizeof(float));
    c[0] = v[0] + rand_uni(-10.f, 10.f);
    return (void*) c;
}

int eval_mc(void* agent)
{
    float* v = (float*) agent;
    return v[0] < 100.f && v[0] > -100.f;
}

void agent_print(void* agent)
{
    float* v = (float*) agent;
    printf("%f", v[0]);
}

int main()
{
    srand( time( NULL ) );

    MCP* mcp = MCP_init(10, 20, 5, sample_viable_agent, resample_agent, free);

    printf("Initial population:\n");
    print_stack(mcp->agents, agent_print);

    int total_generations = 100;
    for (int i = 0; i < total_generations; i++)
    {
        MCP_step(mcp, eval_mc);
    }

    printf("Final population:\n");
    print_stack(mcp->agents, agent_print);
    
    MCP_free(mcp);
    return 0;
}
