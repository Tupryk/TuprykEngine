#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "genetic.h"


struct int_queue
{
    int value;
    struct int_queue* next;
};

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

void reset_agent(struct agent* a)
{
    for (int i = 0; i < a->node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
    }
}

struct agent* mutate_agent(struct population* pop, struct agent* parent)
{
    struct agent* child = (struct agent*) malloc(sizeof(struct agent)); 
    
    int node_count = parent->node_count;
    child->node_count = node_count;

    child->gene_count = parent->gene_count;
    child->genes = (struct gene**) malloc(sizeof(struct gene*) * parent->gene_count);
    for (int i = 0; i < parent->gene_count; i++)
    {
        child->genes[i] = parent->genes[i];
    }
    child->activations = (float*) malloc(sizeof(float) * node_count);
    child->activation_count = (int*) malloc(sizeof(int) * node_count);
    child->activation_funcs = (int*) malloc(sizeof(int) * node_count);

    child->connection_counts = (int*) malloc(sizeof(int) * node_count);
    child->connections = (int**) malloc(sizeof(int*) * node_count);
    child->connection_weight = (float**) malloc(sizeof(float*) * node_count);
    
    for (int i = 0; i < node_count; i++)
    {
        child->activations[i] = 0.f;
        child->activation_count[i] = 0;
        child->activation_funcs[i] = parent->activation_funcs[i];
        child->connection_counts[i] = parent->connection_counts[i];
        if (parent->connection_counts[i] <= 0)
        {
            child->connections[i] = NULL;
            child->connection_weight[i] = NULL;
        }
        else
        {
            child->connections[i] = (int*) malloc(sizeof(int) * parent->connection_counts[i]);
            for (int j = 0; j < parent->connection_counts[i]; j++)
            {
                child->connections[i][j] = parent->connections[i][j];
                child->connection_weight[i][j] = parent->connection_weight[i][j];
            }
        }
    }

    return child;
}

struct population* init_population(int in_dim, int out_dim)
{
    struct population* pop = (struct population*) malloc(sizeof(struct population));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;
    pop->max_size = 90;
    pop->keep_best_n = 30;
    pop->current_size = pop->keep_best_n;
    pop->innovation_count = 2;
    pop->agent_children_count = 2;
    
    pop->innovations = basic_layer_gene(in_dim);
    pop->innovations->next = basic_layer_gene(out_dim);
    pop->latest_innovation = pop->innovations->next;

    pop->agents = (struct agent**) malloc(sizeof(struct agent*) * pop->max_size);
    
    int initial_node_count = in_dim + out_dim;
    for (int i = 0; i < pop->current_size; i++)
    {
        struct agent* new_agent = (struct agent*) malloc(sizeof(struct agent)); 
        new_agent->node_count = initial_node_count;
        
        new_agent->gene_count = 2;
        new_agent->genes = (struct gene**) malloc(sizeof(struct gene*) * 2);
        new_agent->genes[0] = pop->innovations;
        new_agent->genes[1] = pop->innovations->next;

        new_agent->activations = (float*) malloc(sizeof(float) * initial_node_count);
        new_agent->activation_funcs = (int*) malloc(sizeof(int) * initial_node_count);
        new_agent->activation_count = (int*) malloc(sizeof(int) * initial_node_count);

        new_agent->connection_counts = (int*) malloc(sizeof(int) * initial_node_count);
        new_agent->connections = (int**) malloc(sizeof(int*) * initial_node_count);
        new_agent->connection_weight = (float**) malloc(sizeof(float*) * initial_node_count);

        for (int j = 0; j < initial_node_count; j++)
        {
            new_agent->activations[j] = 0.f;
            new_agent->activation_funcs[j] = 0;
            new_agent->activation_count[j] = 0;

            new_agent->connection_counts[j] = 0;
            new_agent->connections[j] = NULL;
            new_agent->connection_weight = NULL;
        }

        pop->agents[i] = new_agent;
    }

    population_mutate(pop);
    
    return pop;
}

void population_mutate(struct population* pop)
{
    int idx = 0;
    int parent_idxs[pop->keep_best_n];
    for (int i = 0; i < pop->max_size; i++)
    {
        if (pop->agents[i] != NULL)
        {
            parent_idxs[idx] = i;
            idx++;
        }
    }
    #ifdef DEBUG
    if (idx != pop->keep_best_n)
    {
        printf("Parents not searched correctly! Expected end index %d and got %d.\n", pop->keep_best_n, idx);
        exit(EXIT_FAILURE);
    }
    #endif

    int latest_null = 0;
    for (int i = 0; i < pop->keep_best_n; i++)
    {
        for (int j = 0; j < pop->agent_children_count; j++)
        {
            while (pop->agents[latest_null] != NULL)
            {
                latest_null++;
                #ifdef DEBUG
                if (latest_null >= pop->max_size)
                {
                    printf("Skipping over max population size!\n");
                    exit(EXIT_FAILURE);
                }
                #endif
            }
            pop->agents[latest_null] = mutate_agent(pop, pop->agents[parent_idxs[i]]);
            pop->innovation_count++;
        }
    }
    printf("Current innovation count: %d\n", pop->innovation_count);
}

void population_kill_weak(struct population* pop, float* scores)
{
    float best_scores[pop->keep_best_n];
    int best_indices[pop->keep_best_n];
    int best_tail = pop->keep_best_n-1;
    for (int i = 0; i < pop->keep_best_n; i++)
    {
        best_scores[i] = -1e6;
        best_indices[i] = -1;
    }

    for (int i = 0; i < pop->max_size; i++)
    {
        int is_good = 0;
        for (int j = 0; j < pop->keep_best_n; j++)
        {
            if(scores[i] >= best_scores[j])
            {
                if (best_indices[best_tail] != -1)
                {
                    agent_free(pop->agents[best_indices[best_tail]]);
                    pop->agents[best_indices[best_tail]] = NULL;
                }
                for (int k = pop->keep_best_n-1; k > j; k--)
                {
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
            agent_free(pop->agents[i]);
            pop->agents[i] = NULL;
        }
    }
    #ifdef DEBUG
    for (int i = 0; i < pop->keep_best_n; i++)
    {
        if (best_indices[i] == -1)
        {
            printf("Best list was not filled!\n");
            exit(EXIT_FAILURE);
        }
    }
    #endif
}

float* feed_agent(struct agent* a, float* input, int in_dim, int out_dim)
{
    reset_agent(a);

    struct int_queue* queue = NULL;
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];

        struct int_queue* tmp = NULL;
        if (queue != NULL)
        {
            tmp = queue->next;
        }
        queue = (struct int_queue*) malloc(sizeof(struct int_queue));
        queue->value = i;
        queue->next = tmp;
    }
    struct int_queue* next_queue = NULL;
    while (queue != NULL)
    {
        struct int_queue* tmp = queue->next;
        free(queue);
        queue = tmp;

        if (queue == NULL)
        {
            queue = next_queue;
        }
    }

    float* out = (float*) malloc(sizeof(float) * out_dim);
    for (int i = 0; i < out_dim; i++)
    {
        out[i] = a->activations[in_dim+i];
    }
    return out;
}

float** population_feed_all_agents(struct population* pop, float* input)
{
    float** out = (float**) malloc(sizeof(float*) * pop->max_size);
    for (int i = 0; i < pop->max_size; i++)
    {
        #ifdef DEBUG
        if (pop->agents[i] == NULL)
        {
            printf("Agents should not be NULL!\n");
            exit(EXIT_FAILURE);
        }
        #endif
        out[i] = feed_agent(pop->agents[i], input, pop->in_dim, pop->out_dim);
    }
    return out;
}

void population_free(struct population* pop)
{
    struct gene* current_gene = pop->innovations;
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
    free(a->activation_count);
    free(a->activation_funcs);
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
