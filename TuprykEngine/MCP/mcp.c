#include <stdio.h>

#include "mcp.h"


MCP* MCP_init(
    int num_seeds,
    int max_size,
    int batch_size,
    void* (*sample_viable_agent)(void),
    void* (*resample_agent)(void*),
    void (*agent_free)(void*)
) {
    MCP* mcp = (MCP*) malloc(sizeof(MCP));
    mcp->max_size = max_size;
    mcp->agents = stack_init();
    mcp->batch_size = batch_size;

    mcp->resample_agent = resample_agent;
    mcp->agent_free = agent_free;
    
    for (int i = 0; i < num_seeds; i++)
    {
        void* agent = sample_viable_agent();  // Must satisfy MC
        stack_push(mcp->agents, agent);
    }
    mcp->current_agent = mcp->agents->first;

    return mcp;
}

void MCP_free(MCP* mcp)
{
    stack_free(mcp->agents, mcp->agent_free);
    free(mcp);
}

void MCP_step(MCP* mcp, int (*eval_mc)(void*))
{
    // Reproduce children and add parents back into queue
    pstack_t* children = stack_init();
    for (int i = 0; i < mcp->batch_size; i++)
    {
        void* child = mcp->resample_agent(mcp->current_agent->data);
        stack_push(children, child);

        if (mcp->current_agent->next)
        {
            mcp->current_agent = mcp->current_agent->next;
        }
        else
        {
            mcp->current_agent = mcp->agents->first;
        }
    }

    // Evaluate children
    while (children->first != NULL)
    {
        void* child = stack_pop_elem(children, children->last);
        int success = eval_mc(child);
        if (success)
        {
            stack_push_between(mcp->agents, child, mcp->agents->last, NULL);
        }
        else
        {
            mcp->agent_free(child);
        }
    }
    stack_free(children, mcp->agent_free);

    // Remove older agents
    int overflow_count = mcp->agents->size - mcp->max_size;
    for (int i = 0; i < overflow_count; i++)
    {
        if (mcp->current_agent == mcp->agents->first)
        {
            mcp->current_agent = mcp->agents->first->next;
        }
        void* agent = stack_pop(mcp->agents);
        mcp->agent_free(agent);
    }
}
