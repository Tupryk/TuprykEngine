#ifndef MCC
#define MCC

#include "../Algos/lists.h"


struct agent_t
{
    void* agent;
    int species;
};

typedef struct
{
    int max_size;
    int batch_size;
    
    void* (*resample_agent)(void*);
    void (*agent_free)(void*);

    pstack_t* agents;
    struct stack_elem* current_agent;
} MCP;

MCP* MCP_init(
    int num_seeds,
    int max_size,
    int batch_size,
    void* (*sample_viable_agent)(void),
    void* (*resample_agent)(void*),
    void (*agent_free)(void*)
);
void MCP_free(MCP* mcp);
void MCP_step(MCP* mcp, int (*eval_mc)(void*));

#endif
