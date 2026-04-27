#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "neat.h"
#include "../../Funcs/funcs.h"
#include "../../Stochastic/sample.h"
#ifdef DEBUG
#include "../../visual/prints/vis_neat.h"
#endif


void reset_agent(agent_t* a)
{
    for (int i = 0; i < a->node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
    }
}

void build_agent_network(population_t* pop, agent_t* a)
{
    int node_count = pop->in_dim + pop->out_dim;
    int* genes = a->genes;
    int gene_count = a->gene_count;
    int pop_to_agent[pop->innovations.size];
    memset(pop_to_agent, -1, sizeof(pop_to_agent));

    for (int i = 0; i < pop->in_dim; i++) pop_to_agent[i] = i;
    for (int i = pop->in_dim; i < node_count; i++) pop_to_agent[i] = i;

    for (int i = 0; i < gene_count; i++)
    {
        if (a->gene_enabled[i])
        {
            gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[i]);
            if (pop_to_agent[gene.out] == -1)
            {
                pop_to_agent[gene.out] = node_count;
                node_count++;
            }
            if (pop_to_agent[gene.in] == -1)
            {
                pop_to_agent[gene.in] = node_count;
                node_count++;
            }
        }
    }

    a->node_count = node_count;
    a->activations = (float*) malloc(sizeof(float) * node_count);
    a->activation_count = (int*) malloc(sizeof(int) * node_count);
    a->activation_funcs = (int*) malloc(sizeof(int) * node_count);
    a->activation_biases = (float*) malloc(sizeof(float) * node_count);
    
    a->connection_counts = (int*) malloc(sizeof(int) * node_count);
    a->connections = (int**) malloc(sizeof(int*) * node_count);
    a->connection_weight = (float**) malloc(sizeof(float*) * node_count);
    
    int connection_counter[node_count];
    int initial_node_count = pop->in_dim + pop->out_dim;
    for (int i = 0; i < node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
        if (i < initial_node_count) a->activation_funcs[i] = 0;
        else a->activation_funcs[i] = 1;
        a->activation_biases[i] = 0.f;
        a->connection_counts[i] = 0;
        connection_counter[i] = 0;
    }

    for (int i = 0; i < a->gene_count; i++)
    {
        gene_t gene = *(gene_t*)vector_get(&pop->innovations, a->genes[i]);
        a->connection_counts[pop_to_agent[gene.out]]++;
    }

    for (int i = 0; i < node_count; i++)
    {
        a->connections[i] = (int*) malloc(sizeof(int) * a->connection_counts[i]);
        a->connection_weight[i] = (float*) malloc(sizeof(float) * a->connection_counts[i]);
    }

    for (int i = 0; i < a->gene_count; i++)
    {
        gene_t gene = *(gene_t*) vector_get(&pop->innovations, a->genes[i]);

        int out = pop_to_agent[gene.out];
        int conn_id = connection_counter[out];
        a->connections[out][conn_id] = pop_to_agent[gene.in];
        a->connection_weight[out][conn_id] = rand_uni(-1.f, 1.f);

        connection_counter[out]++;
    }
}

population_t* init_population(int in_dim, int out_dim)
{
    population_t* pop = (population_t*) malloc(sizeof(population_t));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;
    pop->max_size = 300;
    pop->keep_best_n = 100;
    pop->agent_children_count = 2;
    // pop->network_size_cost_weight = 0.0000001;
    pop->network_size_cost_weight = 0.0;
    pop->mutation_prob = .1;
    
    #ifdef DEBUG
    printf("Initializing population with dimensions; in: %d, out: %d\n", in_dim, out_dim);
    #endif
    pop->innovations = vector_create(sizeof(gene_t));
    for (int i = 0; i < in_dim; i++)
    {
        for (int j = 0; j < out_dim; j++)
        {
            gene_t new_gene;
            new_gene.out = i;
            new_gene.in = j + in_dim;
            vector_push(&pop->innovations, &new_gene);
        }
    }
    #ifdef DEBUG
    printf("Innovations before first mutation: \n");
    print_innovations(&pop->innovations);
    #endif

    pop->agents = (agent_t**) malloc(sizeof(agent_t*) * pop->max_size);
    
    int initial_weight_count = in_dim * out_dim;

    for (int i = 0; i < pop->max_size; i++)
    {
        if (i < pop->keep_best_n)
        {
            agent_t* new_agent = (agent_t*) malloc(sizeof(agent_t)); 
            new_agent->weight_count = initial_weight_count;
            
            int mutate = rand_uni(0.f, 1.f) < pop->mutation_prob ? 1 : 0;

            new_agent->gene_count = initial_weight_count;
            new_agent->genes = (int*) malloc(sizeof(int) * initial_weight_count);
            new_agent->gene_enabled = (int*) malloc(sizeof(int) * initial_weight_count);
            for (int j = 0; j < initial_weight_count; j++)
            {
                new_agent->genes[j] = j;
                new_agent->gene_enabled[j] = 1;
            }
    
            if (mutate) mutate_agent(pop, new_agent);

            build_agent_network(pop, new_agent);
            pop->agents[i] = new_agent;
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

int population_maybe_add_gene(population_t* pop, gene_t* mutation)
{
    // If the gene already exists, return its corresponding index, otherwise create a new gene.
    for (size_t i = 0; i < pop->innovations.size; i++)
    {
        gene_t gene = *(gene_t*) vector_get(&pop->innovations, i);
        if (gene.in == mutation->in && gene.out == mutation->out)
        {
            return i;
        }
    }
    vector_push(&pop->innovations, mutation);
    return pop->innovations.size-1;
}

void mutate_agent(population_t* pop, agent_t* a)
{
    // Agent network should not be initiated!
    int* genes = a->genes;
    int gene_count = a->gene_count;

    int node_count = 0;
    int node_numbers[gene_count];
    int newest_node_in_sequence = 0;
    int in_list[pop->innovations.size];
    memset(in_list, 0, sizeof(in_list));

    for (int i = 0; i < gene_count; i++)
    {
        gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[i]);
        if (!in_list[gene.out])
        {
            in_list[gene.out] = 1;
            node_numbers[node_count] = gene.out;
            node_count++;
            if (newest_node_in_sequence < gene.out) newest_node_in_sequence = gene.out;
        }
        if (!in_list[gene.in])
        {
            in_list[gene.in] = 1;
            node_numbers[node_count] = gene.in;
            node_count++;
            if (newest_node_in_sequence < gene.in) newest_node_in_sequence = gene.in;
        }
    }
    newest_node_in_sequence++;

    gene_t mutation1;
    gene_t mutation2;
    int mutating = 1;
    int mutation_type;
    while (mutating)
    {
        mutating = 0;
        mutation_type = rand() % 2;
        if (mutation_type)
        {
            // Add weight
            int out_node = rand() % node_count;
            int in_node = rand() % (node_count-1);
            if (in_node >= out_node) in_node++;

            mutation1.out = out_node;
            mutation1.in = in_node;

            for (int i = 0; i < gene_count; i++)
            {
                gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[i]);
                if (gene.in == mutation1.in && gene.out == mutation1.out)
                {
                    mutating = 1;
                    break;
                }
            }
        }
        else
        {
            // Add node
            int gene_id = rand() % gene_count;
            gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[gene_id]);

            a->gene_enabled[gene_id] = 0;

            mutation1.out = gene.out;
            mutation1.in = newest_node_in_sequence;

            mutation2.out = newest_node_in_sequence;
            mutation2.in = gene.in;
        }
    }

    // Overwrite genes and return new node count
    gene_count += mutation_type ? 1 : 2;
    
    genes = realloc(genes, sizeof(int) * gene_count);
    a->gene_enabled = realloc(a->gene_enabled, sizeof(int) * gene_count);
    if (mutation_type)
    {
        genes[gene_count-1] = population_maybe_add_gene(pop, &mutation1);
        a->gene_enabled[gene_count-1] = 1;
    }
    else
    {
        genes[gene_count-2] = population_maybe_add_gene(pop, &mutation1);
        genes[gene_count-1] = population_maybe_add_gene(pop, &mutation2);
        a->gene_enabled[gene_count-2] = 1;
        a->gene_enabled[gene_count-1] = 1;
    }
    a->gene_count = gene_count;
}

// agent_t* cross_agents(population_t* pop, agent_t* dominant_parent, agent_t* regular_parent)
// {
//     agent_t* new_agent = (agent_t*) malloc(sizeof(agent)); 
//     new_agent->node_count = ;
//     new_agent->weight_count = ;
    
//     new_agent->gene_count = initial_weight_count;
//     new_agent->genes = (int*) malloc(sizeof(int) * initial_weight_count);
//     new_agent->gene_enabled = (int*) malloc(sizeof(int) * initial_weight_count);
//     for (int j = 0; j < initial_weight_count; j++)
//     {
//         new_agent->genes[j] = j;
//         new_agent->gene_enabled[j] = 1;
//     }

//     return new_agent;
// }

// void population_mutate(population_t* pop)
// {
//     int idx = 0;
//     int parent_idxs[pop->keep_best_n];  // TODO: size_t type instead?
//     for (int i = 0; i < pop->max_size; i++)
//     {
//         if (pop->agents[i] != NULL)
//         {
//             parent_idxs[idx] = i;
//             idx++;
//         }
//     }
//     #ifdef DEBUG
//     if (idx != pop->keep_best_n)
//     {
//         printf("Parents not searched correctly! Expected end index %d and got %d.\n", pop->keep_best_n, idx);
//         exit(EXIT_FAILURE);
//     }
//     #endif

//     int latest_null = 0;
//     for (int i = 0; i < pop->keep_best_n; i++)
//     {
//         for (int j = 0; j < pop->agent_children_count; j++)
//         {
//             while (pop->agents[latest_null] != NULL)
//             {
//                 latest_null++;
//                 #ifdef DEBUG
//                 if (latest_null >= pop->max_size)
//                 {
//                     printf("Skipping over max population size!\n");
//                     exit(EXIT_FAILURE);
//                 }
//                 #endif
//             }
//             #ifdef DEBUG
//             printf("Mutating child %d with parent %d\n", latest_null, parent_idxs[i]);
//             #endif
//             pop->agents[latest_null] = cross_agents(
//                 pop,
//                 pop->agents[dominant_parent_idx],
//                 pop->agents[regular_parent_idx]
//             );
//             mutate_agent(pop, pop->agents[latest_null]);
//             build_agent_network(pop, pop->agents[latest_null]);
//         }
//     }
//     #ifdef DEBUG
//     printf("Innovations after mutation:\n");
//     print_innovations(&pop->innovations);
//     #endif
// }

float* feed_agent(agent_t* a, float* input, int in_dim, int out_dim)
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
    int_stack* stack = int_stack_init();
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        a->activation_count[i] = 1;
        int_stack_push(stack, i);
    }

    #ifdef DEBUG
    int propagation_count = 1;
    printf("Starting propagation 1, nodes... ");
    #endif

    // Forward through the network
    // TODO: Stack usage is wrong here, look at graphics implementation
    int_stack* next_stack = int_stack_init();
    while (stack->size > 0)
    {
        int node_id = int_stack_pop(stack);
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
            if (!a->activation_count[forward_node] && !int_stack_contains(next_stack, forward_node))
            {
                a->activation_count[forward_node]++;
                int_stack_push(next_stack, forward_node);
            }
        }

        if (stack->size <= 0)
        {
            int_stack_free(stack);
            stack = next_stack;
            next_stack = NULL;
            #ifdef DEBUG
            printf("\n");
            if (stack != NULL)
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

float** population_feed_all_agents(population_t* pop, float* input)
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

void population_free(population_t* pop)
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

void free_agent_network(agent_t* a)
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

void agent_free(agent_t* a)
{
    if (a->activations != NULL)
    {
        free_agent_network(a);
    }
    free(a->genes);
    free(a->gene_enabled);
    free(a);
}
