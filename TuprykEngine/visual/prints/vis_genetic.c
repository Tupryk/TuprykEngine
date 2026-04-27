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
    gene g = *(gene_t*)vector_get(innovations, i);
    printf("Gene %zu: ", i);
    printf("Type - ");
    switch (g.type)
    {
    case 0:
    {
        printf("Initial Neurons");
        break;
    }

    case 1:
    {
        new_weight_gene_t* gene_data = (new_weight_gene_t*) g.data;
        printf("New synapse: (%d -> %d: %f)", gene_data->start_idx, gene_data->end_idx, gene_data->weight);
        break;
    }

    case 2:
    {
        weight_perturvation_gene_t* gene_data = (weight_perturvation_gene_t*) g.data;
        printf("Mutate synapse: (%d -> c%d: %f)", gene_data->start_idx, gene_data->connection_idx, gene_data->weight_perturvation);
        break;
    }

    case 3:
    {
        new_node_gene_t* gene_data = (new_node_gene_t*) g.data;
        printf("New Neuron: (%d -> c%d: ", gene_data->weight_start_idx, gene_data->weight_connection_idx);
        print_activation_name(gene_data->activation_type);
        printf("(%f)", gene_data->bias);
        break;
    }

    case 4:
    {
        node_perturvation_gene_t* gene_data = (node_perturvation_gene_t*) g.data;
        printf("Perturb Neuron: (%d -> ", gene_data->neuron_id);
        print_activation_name(gene_data->activation_type);
        printf("(%f)", gene_data->bias);
        break;
    }
    
    default:
        printf("Unknown");
        break;
    }
    printf("\n");
}

void print_agents(agent_t** agents, int count)
{
    for (int i = 0; i < count; i++)
    {
        agent_t* a = agents[i];
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

void print_agent(agent_t* a)
{
    printf("Agent Genes (total: %d) -> ", a->gene_count);
    for (int i = 0; i < a->gene_count; i++)
    {
        printf("%d, ", a->genes[i]);
    }
    printf("\n");
    printf("Total Nodes: %d\n", a->node_count);
    printf("Total Weights: %d\n", a->weight_count);
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

void print_agent_genes(vector* innovations, agent_t* a)
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

void print_population_best_agent(population_t* pop, float* scores)
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
}
