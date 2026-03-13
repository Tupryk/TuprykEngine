#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "genetic.h"


struct gene* basic_layer_gene(int dim)
{
    struct gene* g = (struct gene*) malloc(sizeof(struct gene));
    g->type = 0;
    g->next = NULL;

    g->data = (struct new_nodes_gene*) malloc(sizeof(struct new_nodes_gene));
    struct new_nodes_gene* data_ = (struct new_nodes_gene*) g->data;
    data_->count = dim;
    data_->connection_count = 0;
    data_->connections = NULL;  // Indices of entire network, not just mutation set!
    data_->connection_weights = NULL;
    data_->activation_type = (int*) malloc(sizeof(int) * data_->count);
    for (int i = 0; i < data_->count; i++)
    {
        data_->activation_type[i] = 0;
    }
    return g;
}

void add_mutated_agent(struct population* pop, struct agent* parent)
{

}

struct population* init_population(int in_dim, int out_dim)
{
    struct population* pop = (struct population*) malloc(sizeof(struct population));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;
    pop->max_size = 90;
    pop->keep_best_n = 30;
    pop->current_size = pop->keep_best_n;
    pop->inovation_count = 2;
    pop->agent_children_count = 2;
    
    pop->inovations = basic_layer_gene(in_dim);
    pop->inovations->next = basic_layer_gene(out_dim);
    pop->latest_inovation = pop->inovations->next;

    pop->agents = (struct agent**) malloc(sizeof(struct agent*) * pop->max_size);
    
    int initial_node_count = in_dim + out_dim;
    int initial_adj_shape[] = {initial_node_count, initial_node_count};
    for (int i = 0; i < pop->current_size; i++)
    {
        struct agent* new_agent = (struct agent*) malloc(sizeof(struct agent)); 
        
        new_agent->gene_count = 2;
        new_agent->genes = (struct gene**) malloc(sizeof(struct gene*) * 2);
        new_agent->activations = (float*) malloc(sizeof(float) * initial_node_count);
        for (int j = 0; j < initial_node_count; j++)
        {
            new_agent->activations[j] = 0.f;
        }
        new_agent->activation_funcs = (int*) malloc(sizeof(int) * initial_node_count);
        for (int j = 0; j < initial_node_count; j++)
        {
            new_agent->activation_funcs[j] = 0;
        }
        new_agent->weights = new_sparse(initial_adj_shape, 2);

        pop->agents[i] = new_agent;
    }

    population_mutate(pop);
    
    return pop;
}

void population_mutate(struct population* pop)
{
    for (int i = 0; i < pop->current_size; i++)
    {
        for (int j = 0; j < pop->agent_children_count; j++)
        {
            add_mutated_agent(pop, pop->agents[i]);
            pop->inovation_count++;
        }
    }
    printf("Current inovation count: %d\n", pop->inovation_count);
}

void population_kill_weak(struct population* pop, float* scores)
{
    float best_scores[pop->keep_best_n];
    int best_indices[pop->keep_best_n];
    int best_tail = pop->keep_best_n-1;
    for (int i = 0; i < pop->keep_best_n; i++)
    {
        best_scores[i] = FLT_MIN;
        best_indices[i] = -1;
    }

    for (int i = 0; i < pop->max_size; i++)
    {
        int is_good = 0;
        for (int j = 0; j < pop->keep_best_n; j++)
        {
            if(scores[i] > best_scores[j])
            {
                if (best_indices[best_tail] != -1)
                {
                    // agent_free(pop->agents[best_indices[best_tail]]);
                    // pop->agents[best_indices[best_tail]] = NULL;
                }
                for (int k = pop->keep_best_n-1; k > j; k--)
                {
                    if (best_indices[k-1] == -1) break;
                    best_scores[k] = best_scores[k-1];
                    best_indices[k] = best_indices[k-1];
                }
                best_scores[j] = scores[i];
                best_indices[j] = i;
                is_good = 1;
                break;
            }
        }
        if (!is_good)
        {
            // agent_free(pop->agents[i]);
            // pop->agents[i] = NULL;
        }
    }
}

float* feed_agent(struct agent* a, float* input, int in_dim, int out_dim)
{
    float* out = (float*) malloc(sizeof(float) * out_dim);
    return out;
}

float** population_feed_all_agents(struct population* pop, float* input)
{
    float** out = (float**) malloc(sizeof(float*) * pop->max_size);
    for (int i = 0; i < pop->max_size; i++)
    {
        out[i] = feed_agent(pop->agents[i], input, pop->in_dim, pop->out_dim);
    }
    return out;
}

void population_free(struct population* pop)
{
    struct gene* current_gene = pop->inovations;
    struct gene* next_gene = current_gene->next;
    while (current_gene != NULL)
    {
        gene_free(current_gene);
        current_gene = next_gene;
        next_gene = current_gene->next;
    }
    
    for (int i = 0; i < pop->current_size; i++)
    {
        agent_free(pop->agents[i]);
    }
    free(pop);
}

void agent_free(struct agent* a)
{
    free(a->genes);
    free(a->activations);
    free(a->activation_funcs);
    sparse_free(a->weights);
    free(a);
}

void gene_free(struct gene* g)
{
    switch(g->type)
    {
        case 0:
        // struct new_nodes_gene* data_ = (struct new_nodes_gene*) init_gene->data;
        // int count;
        // int connection_count;
        // int** connections;
        // float* connection_weights;
        // int* activation_type;
        break;
        case 1:
        break;
        case 2:
        break;
    }
    free(g);
}
