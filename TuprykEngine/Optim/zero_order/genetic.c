#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "genetic.h"
#include "../../Stochastic/sample.h"


struct int_queue
{
    int value;
    struct int_queue* next;
};

struct int_queue* queue_add_val(struct int_queue* queue, int value)
{
    struct int_queue* tmp = NULL;
    if (queue != NULL)
    {
        tmp = queue->next;
    }
    struct int_queue* new_start = (struct int_queue*) malloc(sizeof(struct int_queue));
    new_start->value = value;
    new_start->next = tmp;
    
    return new_start;
}

int value_in_queue(struct int_queue* queue, int value)
{
    struct int_queue* tmp = queue;
    while (tmp != NULL)
    {
        if (tmp->value == value) return 1;
        tmp = tmp->next;
    }
    
    return 0;
}

// TODO: move this somewhere else
float relu(float x)
{
    return x < 0.f ? x : 0.f;
}

gene basic_layer_gene(int dim)
{
    gene g;
    g.type = 0;

    g.data = (new_nodes_gene*) malloc(sizeof(new_nodes_gene));
    new_nodes_gene* data_ = (new_nodes_gene*) g.data;
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

void reset_agent(agent* a)
{
    for (int i = 0; i < a->node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
    }
}

void build_agent_network(population* pop, agent* a)
{
    // Initial allocation based on node count
    int node_count = a->node_count;

    a->activations = (float*) malloc(sizeof(float) * node_count);
    a->activation_count = (int*) malloc(sizeof(int) * node_count);
    a->activation_funcs = (int*) malloc(sizeof(int) * node_count);

    a->connection_counts = (int*) malloc(sizeof(int) * node_count);
    a->connections = (int**) malloc(sizeof(int*) * node_count);
    a->connection_weight = (float**) malloc(sizeof(float*) * node_count);

    for (int i = 0; i < node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
        a->activation_funcs[i] = 0;
    }

    // Counting weights
    for (int i = 0; i < a->gene_count; i++)
    {
        gene check_gene = *(gene*)vector_get(&pop->innovations, i);
        if (check_gene.type == 0)
        {

        }
    }

    // Allocate memory for weights
    for (int i = 0; i < node_count; i++)
    {
        if (a->connection_counts[i] <= 0)
        {
            a->connections[i] = NULL;
            a->connection_weight[i] = NULL;
        }
        else
        {
            a->connections[i] = (int*) malloc(sizeof(int) * a->connection_counts[i]);
            a->connection_weight[i] = (float*) malloc(sizeof(float) * a->connection_counts[i]);
        }
    }

    // Set weight values
    for (int i = 0; i < a->gene_count; i++)
    {

    }
}

void free_agent_network(agent* a)
{
    free(a->activations);
    free(a->activation_count);
    free(a->activation_funcs);

    for (int i = 0; i < a->node_count; i++)
    {
        if (a->connection_counts[i] > 0)
        {
            free(a->connections[i]);
            free(a->connection_weight[i]);
        }
    }

    free(a->connections);
    free(a->connection_counts);
    free(a->connection_weight);
}

agent* mutate_agent(population* pop, agent* parent)
{
    // Copy parent on child
    agent* child = (agent*) malloc(sizeof(agent));

    child->node_count = parent->node_count;

    child->gene_count = parent->gene_count;
    child->genes = (int*) malloc(sizeof(int) * parent->gene_count);
    for (int i = 0; i < parent->gene_count; i++)
    {
        child->genes[i] = parent->genes[i];
    }

    // Mutate child
    gene new_gene;
    int mutation_done = 0;
    while (!mutation_done)
    {
        mutation_done = 1;
        int mutation_type = rand() % 3;
        new_gene.type = mutation_type;
        switch (mutation_type)
        {
        case 0:
        {
            // Add weight
            int start_node = rand() % child->node_count;
            int end_node = rand() % child->node_count;
            float weight = rand_uni(-1.f, 1.f);

            new_weight_gene* gene_data = (new_weight_gene*) malloc(sizeof(new_weight_gene));
            gene_data->start_idx = start_node;
            gene_data->end_idx = end_node;
            gene_data->weight = weight;

            new_gene->data = gene_data;

            break;
        }
    
        case 1:
        {
            // Mutate weight
            int start_node = rand() % child->node_count;
            int cc = child->connection_counts[start_node];
            if (cc > 0)
            {
                int connection_idx = rand() % cc;
                float new_weight = rand_uni(-1.f, 1.f);

                weight_perturvation_gene* gene_data = (weight_perturvation_gene*) malloc(sizeof(weight_perturvation_gene));
                gene_data->start_idx = start_node;
                gene_data->connection_idx = connection_idx;
                gene_data->new_weight = new_weight;

                new_gene->data = gene_data;
            }
            else mutation_done = 0;
            break;
        }

        // case 2:
        // {
        //     child->node_count++;
        //     break;
        // }
        
        default:
            #ifdef DEBUG
            printf("Mutation type does not exist!\n");
            exit(EXIT_FAILURE);
            #endif
            break;
        }
    }
    vector_push(&pop->innovations, &new_gene);

    return child;
}

population* init_population(int in_dim, int out_dim)
{
    population* pop = (population*) malloc(sizeof(population));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;
    pop->max_size = 300;
    pop->keep_best_n = 100;
    pop->current_size = pop->keep_best_n;
    pop->agent_children_count = 2;
    
    pop->innovations = vector_create(sizeof(gene));
    gene in_gene = basic_layer_gene(in_dim);
    gene out_gene = basic_layer_gene(out_dim);
    vector_push(&pop->innovations, &in_gene);
    vector_push(&pop->innovations, &out_gene);

    pop->agents = (agent**) malloc(sizeof(agent*) * pop->max_size);
    
    int initial_node_count = in_dim + out_dim;
    for (int i = 0; i < pop->current_size; i++)
    {
        agent* new_agent = (agent*) malloc(sizeof(agent)); 
        new_agent->node_count = initial_node_count;
        
        new_agent->gene_count = 2;
        new_agent->genes = (int*) malloc(sizeof(int) * 2);
        new_agent->genes[0] = 0;
        new_agent->genes[1] = 1;

        pop->agents[i] = new_agent;
    }

    population_mutate(pop);
    
    return pop;
}

void population_mutate(population* pop)
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
        }
    }
}

void population_kill_weak(population* pop, float* scores)
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

float* feed_agent(agent* a, float* input, int in_dim, int out_dim)
{
    #if DEBUG
    if (a->activations == NULL)
    {
        printf("Agent network was not initialized!\n")
        exit(EXIT_FAILURE);
    }
    #endif

    // Initialize the input layer
    struct int_queue* queue = NULL;
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        queue = queue_add_val(queue, i);
    }

    // Forward through the network
    struct int_queue* next_queue = NULL;
    while (queue != NULL)
    {
        int node_id = queue->value;
        a->activation_count[node_id]++;
        // Pass through non-linearity
        float node_activation = a->activations[node_id];
        float forward_act = node_activation;
        switch (a->activation_funcs[node_id])
        {
        case 0:
            // Nothing
            break;

        case 1:
            forward_act = relu(forward_act);
            break;

        default:
            #ifdef DEBUG
            printf("Invalid node activation function!\n");
            #endif
            break;
        }

        for (int i = 0; i < a->connection_counts[node_id]; i++)
        {
            int forward_node = a->connections[node_id][i];
            float w = a->connection_weight[node_id][i];
            a->activations[forward_node] += w * forward_act;
            if (!value_in_queue(queue, i) && !a->activation_count[i])
            {
                queue = queue_add_val(queue, i);
            }
        }

        struct int_queue* tmp = queue->next;
        free(queue);
        queue = tmp;

        if (queue == NULL)
        {
            queue = next_queue;
        }
    }

    // Write to output array
    float* out = (float*) malloc(sizeof(float) * out_dim);
    for (int i = 0; i < out_dim; i++)
    {
        out[i] = a->activations[in_dim+i];
    }

    return out;
}

float** population_feed_all_agents(population* pop, float* input)
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
        build_agent_network(pop, pop->agents[i]);
        out[i] = feed_agent(pop->agents[i], input, pop->in_dim, pop->out_dim);
        free_agent_network(pop->agents[i]);
    }
    return out;
}

void population_free(population* pop)
{
    for (size_t i = 0; i < pop->innovations.size; i++)
    {
        gene_free(*(gene*)vector_get(&pop->innovations, i));
    }
    
    for (int i = 0; i < pop->current_size; i++)
    {
        agent_free(pop->agents[i]);
    }
    free(pop);
}

void agent_free(agent* a)
{
    if (a->activations != NULL)
    {
        free_agent_network(a);
    }
    free(a->genes);
    free(a);
}

void gene_free(gene g)
{
    switch(g.type)
    {
    case 0:
        free(g.data);
        break;

    case 1:
        free(g.data);
        break;

    case 2:
        break;
    }
}
