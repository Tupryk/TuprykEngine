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
    {
        printf("New neurons");
        break;
    }

    case 1:
    {
        new_weight_gene* wg = (new_weight_gene*) g.data;
        printf("New synapse: (%d -> %d: %f)", wg->start_idx, wg->end_idx, wg->weight);
        break;
    }

    case 2:
    {
        weight_perturvation_gene* wp = (weight_perturvation_gene*) g.data;
        printf("Mutate synapse: (%d -> c%d: %f)", wp->start_idx, wp->connection_idx, wp->new_weight);
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
    printf("Agent Genes (total: %d) -> ", a->gene_count);
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    printf("Agent Node Connections: \n");
    for (int i = 0; i < a->node_count; i++)
    {
        printf("%d (", i);
        switch (a->activation_funcs[i])
        {
        case 0:
            printf("Linear");
            break;

        case 1:
            printf("ReLU");
            break;
        
        case 2:
            printf("LeakyReLU");
            break;

        case 3:
            printf("Sigmoid");
            break;

        default:
            #ifdef DEBUG
            printf("Invalid node activation function!\n");
            exit(EXIT_FAILURE);
            #endif
            break;
        }
        printf("): ");
        for (int j = 0; j < a->connection_counts[i]; j++)
        {
            printf("%d (%f), ", a->connections[i][j], a->connection_weight[i][j]);
        }
        printf("\n");
    }
}

void print_agent_genes(vector* innovations, agent* a)
{
    printf("Agent Genes (total: %d) -> ", a->gene_count);
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

void print_population_best_agent(population* pop, float* scores)
{
    printf("+------------------------+\n");
    float best_score = scores[0];
    int best_agent_idx = 0;
    for (int i = 1; i < pop->max_size; i++)
    {
        if (best_score < scores[i])
        {
            best_score = scores[i];
            best_agent_idx = i;
        }
    }

    printf("Best agent in population (with score: %f and id: %d)\n", best_score, best_agent_idx);
    int agent_built = pop->agents[best_agent_idx]->activations != NULL;
    if (!agent_built)
    {
        build_agent_network(pop, pop->agents[best_agent_idx]);
    }
    print_agent_genes(&pop->innovations, pop->agents[best_agent_idx]);
    print_agent(pop->agents[best_agent_idx]);
    if (!agent_built)
    {
        free_agent_network(pop->agents[best_agent_idx]);
    }
    printf("+------------------------+\n");
}
