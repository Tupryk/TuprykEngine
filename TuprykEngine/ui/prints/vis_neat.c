#include <stdio.h>
#include <string.h>

#include "vis_neat.h"


void print_innovations(vector* innovations)
{
    for (size_t i = 0; i < innovations->size; i++)
    {
        print_innovation(innovations, i);
    }
}

void print_innovation(vector* innovations, size_t i)
{
    gene_t g = *(gene_t*)vector_get(innovations, i);
    printf("Gene %zu: %d -> %d\n", i, g.out, g.in);
}

void print_agents(population_t* pop)
{
    for (int i = 0; i < pop->size; i++)
    {
        agent_t* a = pop->agents[i];
        if (a == NULL) {
            continue;
        }
        
        printf("Agent %d (species %d): Genes -> ", i, pop->agent_to_species[i]);
        for (int j = 0; j < a->gene_count; j++)
        {
            printf("%d, ", a->genes[j]);
        }
        printf("\n");
    }
}

void print_agent(agent_t* a)
{
    printf("Agent Genes (total: %d) -> ", a->gene_count);
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    printf("Total Nodes: %d\n", a->node_count);
    printf("Total Genes: %d\n", a->gene_count);
    printf("Agent Node Connections: \n");
    for (int i = 0; i < a->node_count; i++)
    {
        printf("%d (", i);
        print_activation_name(a->activation_funcs[i]);
        printf("): ");
        for (int j = 0; j < a->connection_counts[i]; j++)
        {
            printf("%d (%f), ", a->connections[i][j], a->connection_weight[i][j]);
        }
        printf("\n");
    }
}

void print_species_info(population_t* pop)
{
    int species_sizes[pop->species.size];
    memset(species_sizes, 0, sizeof(species_sizes));

    printf("Total species: %zu\n", pop->species.size);
    for (int i = 0; i < pop->size; i++)
    {
        int species_id = pop->agent_to_species[i];
        if (species_id != -1)
        {
            species_sizes[species_id]++;
        }
    }
    for (size_t i = 0; i < pop->species.size; i++)
    {
        printf("Species %zu size: %d\n", i, species_sizes[i]);
    }
}

void print_agent_gene_sequence(vector* innovations, agent_t* a)
{
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("|    ");
        if (a->genes[i] < 10) printf(" ");
        printf("(%d)    ", a->genes[i]);
    }
    printf("|\n");
    for (int i = 0; i < a->gene_count; i++)
    {
        gene_t g = *(gene_t*) vector_get(innovations, a->genes[i]);
        printf("|  %d  ->  %d  ", g.out, g.in);
    }
    printf("|\n");
    for (int i = 0; i < a->gene_count; i++)
    {
        float w = a->gene_weights[i];
        printf("|   ");
        if (w >= 0.f) printf(" ");
        printf("%.2f    ", w);
    }
    printf("|\n");
    for (int i = 0; i < a->gene_count; i++)
    {
        if (!a->gene_enabled[i]) printf("| (DISABLED) ");
        else printf("|            ");
    }
    printf("|\n");
}

void print_agent_genes(vector* innovations, agent_t* a)
{
    printf("Agent Genes (total: %d) -> ", a->gene_count);
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    print_agent_gene_sequence(innovations, a);
}

void print_population_best_agent(population_t* pop, float* scores)
{
    printf("+------------------------+\n");
    float best_score = scores[0];
    int best_agent_idx = 0;
    for (int i = 1; i < pop->size; i++)
    {
        if (best_score < scores[i])
        {
            best_score = scores[i];
            best_agent_idx = i;
        }
    }

    printf("Best agent in population (with score: %f and id: %d)\n", best_score, best_agent_idx);
    print_agent_genes(&pop->innovations, pop->agents[best_agent_idx]);
    print_agent(pop->agents[best_agent_idx]);
    printf("+------------------------+\n");
}

void print_activation_name(int act_id)
{
    switch (act_id)
    {
    case 0:
        printf("Linear");
        break;

    case 1:
        printf("ReLU");
        break;

    default:
        #ifdef DEBUG
        printf("Invalid node activation function!\n");
        exit(EXIT_FAILURE);
        #endif
        break;
    }
}
