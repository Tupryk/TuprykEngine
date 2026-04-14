#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "neat.h"
#include "../../Funcs/funcs.h"
#include "../../Stochastic/sample.h"
#ifdef DEBUG
#include "../../visual/prints/vis_neat.h"
#endif


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
    a->activation_biases = (float*) malloc(sizeof(float) * node_count);
    
    a->connection_counts = (int*) malloc(sizeof(int) * node_count);
    a->connections = (int**) malloc(sizeof(int*) * node_count);
    a->connection_weight = (float**) malloc(sizeof(float*) * node_count);
    
    int initial_node_count = pop->in_dim + pop->out_dim;
    for (int i = 0; i < node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
        if (i < initial_node_count) a->activation_funcs[i] = 0;
        else a->activation_funcs[i] = 1;
        a->activation_biases[i] = 0.f;
    }
}

population* init_population(int in_dim, int out_dim)
{
    population* pop = (population*) malloc(sizeof(population));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;
    pop->max_size = 300;
    pop->keep_best_n = 100;
    pop->agent_children_count = 2;
    // pop->network_size_cost_weight = 0.0000001;
    pop->network_size_cost_weight = 0.0;
    
    #ifdef DEBUG
    printf("Initializing population with dimensions; in: %d, out: %d\n", in_dim, out_dim);
    #endif
    pop->innovations = vector_create(sizeof(gene));
    for (int i = 0; i < in_dim; i++)
    {
        for (int j = 0; j < out_dim; j++)
        {
            gene new_gene;
            new_gene.in = i;
            new_gene.out = j;
            vector_push(&pop->innovations, &new_gene);
        }
    }
    #ifdef DEBUG
    printf("Innovations before first mutation: \n");
    print_innovations(&pop->innovations);
    #endif

    pop->agents = (agent**) malloc(sizeof(agent*) * pop->max_size);
    
    int initial_node_count = in_dim + out_dim;
    int initial_weight_count = in_dim * out_dim;

    pop->node_counter = initial_node_count;

    for (int i = 0; i < pop->max_size; i++)
    {
        if (i < pop->keep_best_n)
        {
            agent* new_agent = (agent*) malloc(sizeof(agent)); 
            new_agent->node_count = initial_node_count;
            new_agent->weight_count = initial_weight_count;
            
            new_agent->gene_count = initial_weight_count;
            new_agent->genes = (int*) malloc(sizeof(int) * initial_weight_count);
            new_agent->gene_enabled = (int*) malloc(sizeof(int) * initial_weight_count);
            for (int j = 0; j < initial_weight_count; j++)
            {
                new_agent->genes[j] = j;
                new_agent->gene_enabled[j] = 1;
            }
    
            pop->agents[i] = new_agent;
            build_agent_network(pop, pop->agents[i]);
        }
        else
        {
            pop->agents[i] = NULL;
        }
    }
    #ifdef DEBUG
    printf("Initial agents:\n");
    print_agents(pop->agents, pop->keep_best_n);
    #endif

    return pop;
}

agent* cross_agents(population* pop, agent* dominant_parent, agent* regular_parent)
{
    agent* new_agent = (agent*) malloc(sizeof(agent)); 
    new_agent->node_count = ;
    new_agent->weight_count = ;
    
    new_agent->gene_count = initial_weight_count;
    new_agent->genes = (int*) malloc(sizeof(int) * initial_weight_count);
    new_agent->gene_enabled = (int*) malloc(sizeof(int) * initial_weight_count);
    for (int j = 0; j < initial_weight_count; j++)
    {
        new_agent->genes[j] = j;
        new_agent->gene_enabled[j] = 1;
    }

    return new_agent;
}

void mutate_agent(population* pop, agent* a)
{
    // TODO IMPORTANT: expand agent gene count!!!
    // TODO: Check if a new gene already exists!!!

    int mutation_type = rand() % 2;
    if (mutation_type)
    {
        // Add weight
        gene new_gene;
        new_gene.in = ;
        new_gene.out = ;
        vector_push(&pop->innovations, &new_gene);
    }
    else
    {
        // Add node
        int choosing_weight = 1;
        while (choosing_weight)
        {
            int chosen_weight = a->gene_count-1;
            if (a->gene_enabled[chosen_weight])
            {
                a->gene_enabled[chosen_weight] = 0;
                
                gene original_weight = *(gene*)vector_get(&pop->innovations, a->genes[chosen_weight]);
                
                gene new_gene_a;
                new_gene_a.in = original_weight.in;
                new_gene_a.out = pop->node_counter;

                gene new_gene_b;
                new_gene_a.in = pop->node_counter;
                new_gene_a.out = original_weight.out;
                
                vector_push(&pop->innovations, &new_gene_a);
                vector_push(&pop->innovations, &new_gene_b);
                
                pop->node_counter++;
                choosing_weight = 0;
            }
        }
    }

    a->genes[a->gene_count - 1] = pop->innovations.size - 1;
}

void population_mutate(population* pop)
{
    int idx = 0;
    int parent_idxs[pop->keep_best_n];  // TODO: size_t type instead?
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
            #ifdef DEBUG
            printf("Mutating child %d with parent %d\n", latest_null, parent_idxs[i]);
            #endif
            pop->agents[latest_null] = cross_agents(
                pop,
                pop->agents[dominant_parent_idx],
                pop->agents[regular_parent_idx]
            );
            mutate_agent(pop, pop->agents[latest_null]);
            build_agent_network(pop, pop->agents[latest_null]);
        }
    }
    #ifdef DEBUG
    printf("Innovations after mutation:\n");
    print_innovations(&pop->innovations);
    #endif
}

float* feed_agent(agent* a, float* input, int in_dim, int out_dim)
{
    reset_agent(a);

    #ifdef DEBUG
    if (a->activations == NULL)
    {
        printf("Agent network was not initialized!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    #ifdef DEBUG
    printf("Initializing activations...\n");
    #endif

    // Initialize the input layer
    struct int_queue* queue = NULL;
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        a->activation_count[i] = 1;
        queue = queue_push(queue, i);
    }

    #ifdef DEBUG
    int propagation_count = 1;
    printf("Starting propagation 1, nodes... ");
    #endif

    // Forward through the network
    struct int_queue* next_queue = NULL;
    while (queue != NULL)
    {
        
        int node_id = queue->value;
        // Pass through non-linearity
        float node_activation = a->activations[node_id];
        float forward_act = node_activation + a->activation_biases[node_id];
        switch (a->activation_funcs[node_id])
        {
        case 0:
            // Linear
            break;

        case 1:
            forward_act = relu(forward_act);
            break;

        default:
            #ifdef DEBUG
            printf("Invalid node activation function!\n");
            exit(EXIT_FAILURE);
            #endif
            break;
        }

        #ifdef DEBUG
        printf("%d", node_id);
        #endif
        for (int i = 0; i < a->connection_counts[node_id]; i++)
        {
            int forward_node = a->connections[node_id][i];
            float w = a->connection_weight[node_id][i];
            a->activations[forward_node] += w * forward_act;
            if (!a->activation_count[forward_node] && !queue_contains(next_queue, forward_node))
            {
                a->activation_count[forward_node]++;
                next_queue = queue_push(next_queue, forward_node);
            }
        }

        struct int_queue* tmp = queue->next;
        free(queue);
        queue = tmp;

        if (queue == NULL)
        {
            queue = next_queue;
            next_queue = NULL;
            #ifdef DEBUG
            printf("\n");
            if (queue != NULL)
            {
                propagation_count++;
                printf("Starting propagation %d, nodes... ", propagation_count);
            }
            else
            {
                printf("Finished propagating.\n");
            }
            #endif
        }
        #ifdef DEBUG
        else
        {
            printf(", ");
        }
        #endif
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
        
        #ifdef DEBUG
        printf("+------------------------+\n");
        print_agent(pop->agents[i]);
        printf("Feeding agent %d: \n", i);
        #endif

        out[i] = feed_agent(pop->agents[i], input, pop->in_dim, pop->out_dim);

        #ifdef DEBUG
        printf("Given input: [");
        for (int j = 0; j < pop->in_dim; j++)
        {
            printf("%f, ", input[j]);
        }
        printf("]\nAgent outputs: [");
        for (int j = 0; j < pop->out_dim; j++)
        {
            printf("%f, ", out[i][j]);
        }
        printf("]\n");
        printf("+------------------------+\n");
        #endif
    }
    return out;
}

void population_free(population* pop)
{
    for (int i = 0; i < pop->max_size; i++)
    {
        if (pop->agents[i] != NULL)
        {
            agent_free(pop->agents[i]);
        }
    }
    vector_free(&pop->innovations);
    free(pop);
}

void free_agent_network(agent* a)
{
    free(a->activations);
    a->activations = NULL;
    free(a->activation_count);
    free(a->activation_funcs);
    free(a->activation_biases);

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

void agent_free(agent* a)
{
    if (a->activations != NULL)
    {
        free_agent_network(a);
    }
    free(a->genes);
    free(a->gene_enabled);
    free(a);
}
