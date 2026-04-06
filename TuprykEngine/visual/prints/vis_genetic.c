#include <stdio.h>

#include "vis_genetic.h"


void print_innovations(vector* innovations)
{
    for (size_t i = 0; i < innovations->size; i++)
    {
        print_innovation(innovations, i);
    }
}

void print_innovation(vector* innovations, size_t i)
{
    gene g = *(gene*)vector_get(innovations, i);
    printf("Gene %zu: ", i);
    printf("Type - ");
    switch (g.type)
    {
    case 0:
        printf("New neurons");
        break;

    case 1:
    {
        new_weight_gene* wg = (new_weight_gene*) g.data;
        printf("New synapse: (%d -> %d: %f)", wg->start_idx, wg->end_idx, wg->weight);
        break;
    }
    
    default:
        printf("Unknown");
        break;
    }
    printf("\n");
}

void print_agents(agent** agents, int count)
{
    for (int i = 0; i < count; i++)
    {
        agent* a = agents[i];
        if (a == NULL) {
            continue;
        }
        
        printf("Agent %d: Genes -> ", i);
        for (int j = 0; j < a->gene_count; j++)
        {
            printf("%d, ", a->genes[j]);
        }
        printf("\n");
    }
}

void print_agent(agent* a)
{
    printf("Agent Genes -> ");
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    printf("Agent Node Connections: \n");
    for (int i = 0; i < a->node_count; i++)
    {
        printf("%d: ", i);
        for (int j = 0; j < a->connection_counts[i]; j++)
        {
            printf("%d (%f), ", a->connections[i][j], a->connection_weight[i][j]);
        }
        printf("\n");
    }
}

void print_agent_genes(vector* innovations, agent* a)
{
    printf("Agent Genes -> ");
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    for (int i = 0; i < a->gene_count; i++)
    {
        print_innovation(innovations, a->genes[i]);
    }

}
