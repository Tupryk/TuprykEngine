#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>

#include "genetic.h"
#include "../../Funcs/funcs.h"
#include "../../Stochastic/sample.h"
#ifdef DEBUG
#include "../../visual/prints/vis_genetic.h"
#endif


gene basic_layer_gene(int dim)
{
    gene g;
    g.type = 0;

    g.data = (new_nodes_gene_t*) malloc(sizeof(new_nodes_gene));
    new_nodes_gene_t* data_ = (new_nodes_gene_t*) g.data;
    data_->count = dim;
    data_->connection_count = 0;
    data_->connections = NULL;  // Indices of entire network, not just mutation set!
    data_->connection_weights = NULL;
    data_->activation_type = (int*) malloc(sizeof(int) * data_->count);
    data_->activation_biases = (float*) malloc(sizeof(float) * data_->count);
    for (int i = 0; i < data_->count; i++)
    {
        data_->activation_type[i] = 0;
        data_->activation_biases[i] = 0.f;
    }
    return g;
}

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
    // Initial allocation based on node count
    int node_count = a->node_count;

    a->activations = (float*) malloc(sizeof(float) * node_count);
    a->activation_count = (int*) malloc(sizeof(int) * node_count);
    a->activation_funcs = (int*) malloc(sizeof(int) * node_count);
    a->activation_biases = (float*) malloc(sizeof(float) * node_count);
    
    int con_counting[node_count];
    a->connection_counts = (int*) malloc(sizeof(int) * node_count);
    a->connections = (int**) malloc(sizeof(int*) * node_count);
    a->connection_weight = (float**) malloc(sizeof(float*) * node_count);
    
    for (int i = 0; i < node_count; i++)
    {
        con_counting[i] = 0;
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
        a->activation_funcs[i] = 0;
        a->activation_biases[i] = 0.f;
    }
    
    // Counting weights and setting up nodes
    int node_counting = 0;
    for (int i = 0; i < a->gene_count; i++)
    {
        gene check_gene = *(gene_t*)vector_get(&pop->innovations, a->genes[i]);
        if (check_gene.type == 0)
        {
            new_nodes_gene_t* gene_data = (new_nodes_gene_t*) check_gene.data;
            node_counting += gene_data->count;
        }
        else if (check_gene.type == 1)
        {
            new_weight_gene_t* gene_data = (new_weight_gene_t*) check_gene.data;
            a->connection_counts[gene_data->start_idx]++;
        }
        else if (check_gene.type == 3)
        {
            new_node_gene_t* gene_data = (new_node_gene_t*) check_gene.data;
            a->activation_funcs[node_counting] = gene_data->activation_type;
            a->activation_biases[node_counting] = gene_data->bias;
            a->connection_counts[node_counting]++;
            node_counting++;
        }
        else if (check_gene.type == 4)
        {
            node_perturvation_gene_t* gene_data = (node_perturvation_gene_t*) check_gene.data;
            a->activation_funcs[gene_data->neuron_id] = gene_data->activation_type;
            a->activation_biases[gene_data->neuron_id] += gene_data->bias;
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
    node_counting = 0;
    for (int i = 0; i < a->gene_count; i++)
    {
        gene check_gene = *(gene_t*)vector_get(&pop->innovations, a->genes[i]);
        if (check_gene.type == 0)
        {
            new_nodes_gene_t* gene_data = (new_nodes_gene_t*) check_gene.data;
            node_counting += gene_data->count;
        }
        if (check_gene.type == 1)
        {
            new_weight_gene_t* wg = (new_weight_gene_t*) check_gene.data;
            
            int con_idx = con_counting[wg->start_idx];
            a->connections[wg->start_idx][con_idx] = wg->end_idx;
            a->connection_weight[wg->start_idx][con_idx] = wg->weight;
            
            con_counting[wg->start_idx]++;
        }
        else if (check_gene.type == 2)
        {
            weight_perturvation_gene_t* wp = (weight_perturvation_gene_t*) check_gene.data;
            a->connection_weight[wp->start_idx][wp->connection_idx] += wp->weight_perturvation;
        }
        else if (check_gene.type == 3)
        {
            new_node_gene_t* gene_data = (new_node_gene_t*) check_gene.data;

            int osn = gene_data->weight_start_idx;
            int ocn = gene_data->weight_connection_idx;

            a->connections[node_counting][0] = a->connections[osn][ocn];
            a->connection_weight[node_counting][0] = a->connection_weight[osn][ocn];

            a->connections[osn][ocn] = node_counting;
            a->connection_weight[osn][ocn] = 1.f;

            con_counting[node_counting]++;
            node_counting++;
        }
    }
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

agent_t* mutate_agent(population_t* pop, agent_t* parent)
{
    // Copy parent on child
    agent_t* child = (agent_t*) malloc(sizeof(agent));

    child->node_count = parent->node_count;
    child->weight_count = parent->weight_count;

    child->gene_count = parent->gene_count + 1;
    child->genes = (int*) malloc(sizeof(int) * child->gene_count);
    for (int i = 0; i < parent->gene_count; i++)
    {
        child->genes[i] = parent->genes[i];
    }

    #ifdef DEBUG
    printf("Mutating child...\nMutation type: ");
    #endif
    // Mutate child
    gene new_gene;
    int mutation_done = 0;
    while (!mutation_done)
    {
        mutation_done = 1;
        int mutation_type = rand() % 5;
        new_gene.type = mutation_type;
        switch (mutation_type)
        {
        case 0:
        {
            // child->node_count++;
            // printf("Add nodes");
            mutation_done = 0;
            break;
        }
        case 1:
        {
            // Add weight
            child->weight_count++;

            int start_node = rand() % child->node_count;
            int end_node = rand() % child->node_count;
            float weight = rand_uni(-1.f, 1.f);

            new_weight_gene_t* gene_data = (new_weight_gene_t*) malloc(sizeof(new_weight_gene));
            gene_data->start_idx = start_node;
            gene_data->end_idx = end_node;
            gene_data->weight = weight;

            new_gene.data = gene_data;

            #ifdef DEBUG
            printf("Add weight (%d -> %d: %f)", gene_data->start_idx, gene_data->end_idx, gene_data->weight);
            #endif
            break;
        }
    
        case 2:
        {
            // Mutate weight
            int start_node = rand() % child->node_count;
            int cc = parent->connection_counts[start_node];
            if (cc > 0)
            {
                int connection_idx = rand() % cc;
                float weight_perturvation = rand_uni(-1.f, 1.f);

                weight_perturvation_gene_t* gene_data = (weight_perturvation_gene_t*) malloc(sizeof(weight_perturvation_gene));
                gene_data->start_idx = start_node;
                gene_data->connection_idx = connection_idx;
                gene_data->weight_perturvation = weight_perturvation;

                new_gene.data = gene_data;

                #ifdef DEBUG
                printf("Mutate weights (%d -> c%d: %f)", gene_data->start_idx, gene_data->connection_idx, gene_data->weight_perturvation);
                #endif
            }
            else mutation_done = 0;
            break;
        }

        case 3:
        {
            // Add neuron
            int start_node = rand() % child->node_count;
            int cc = parent->connection_counts[start_node];
            if (cc > 0)
            {
                child->node_count++;
                child->weight_count++;

                new_node_gene_t* gene_data = (new_node_gene_t*) malloc(sizeof(new_node_gene));
                gene_data->weight_start_idx = start_node;
                gene_data->weight_connection_idx = rand() % cc;
                gene_data->activation_type = 2;
                gene_data->bias = rand_uni(-1.f, 1.f);

                new_gene.data = gene_data;

                #ifdef DEBUG
                printf("Add neuron (%d -> c%d: %f)", gene_data->start_idx, gene_data->connection_idx, gene_data->weight_perturvation);
                #endif
            }
            else mutation_done = 0;
            break;
        }

        case 4:
        {
            // Perturb neuron
            node_perturvation_gene_t* gene_data = (node_perturvation_gene_t*) malloc(sizeof(node_perturvation_gene));
            gene_data->neuron_id = rand() % child->node_count;
            gene_data->activation_type = rand() % 4;
            gene_data->bias = rand_uni(-1.f, 1.f);
            
            new_gene.data = gene_data;

            #ifdef DEBUG
            printf("Perturb neuron (%d: %d(%f))", gene_data->neuron_id, gene_data->activation_type, gene_data->bias);
            #endif
            break;
        }
        
        default:
            #ifdef DEBUG
            printf("Mutation type does not exist!\n");
            exit(EXIT_FAILURE);
            #endif
            break;
        }
    }
    #ifdef DEBUG
    printf("\n");
    #endif

    child->genes[parent->gene_count] = pop->innovations.size;
    vector_push(&pop->innovations, &new_gene);

    return child;
}

population_t* init_population(int in_dim, int out_dim)
{
    population_t* pop = (population_t*) malloc(sizeof(population));
    
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
    gene in_gene = basic_layer_gene(in_dim);
    gene out_gene = basic_layer_gene(out_dim);
    vector_push(&pop->innovations, &in_gene);
    vector_push(&pop->innovations, &out_gene);
    #ifdef DEBUG
    printf("Innovations before first mutation: \n");
    print_innovations(&pop->innovations);
    #endif

    #ifdef DEBUG
    printf("Initializing agents...\n");
    #endif
    pop->agents = (agent_t**) malloc(sizeof(agent_t*) * pop->max_size);
    
    int initial_node_count = in_dim + out_dim;
    for (int i = 0; i < pop->keep_best_n; i++)
    {
        agent_t* new_agent = (agent_t*) malloc(sizeof(agent)); 
        new_agent->node_count = initial_node_count;
        new_agent->weight_count = 0;
        
        new_agent->gene_count = 2;
        new_agent->genes = (int*) malloc(sizeof(int) * 2);
        new_agent->genes[0] = 0;
        new_agent->genes[1] = 1;

        pop->agents[i] = new_agent;
        build_agent_network(pop, pop->agents[i]);
    }
    #ifdef DEBUG
    printf("First 10 agents before first mutation:\n");
    print_agents(pop->agents, pop->max_size);
    #endif

    population_mutate(pop);

    #ifdef DEBUG
    printf("Innovations after first mutation:\n");
    print_innovations(&pop->innovations);
    printf("First 10 agents after first mutation:\n");
    print_agents(pop->agents, pop->max_size);
    #endif
    
    return pop;
}

void population_mutate(population_t* pop)
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
            pop->agents[latest_null] = mutate_agent(pop, pop->agents[parent_idxs[i]]);
            build_agent_network(pop, pop->agents[latest_null]);
        }
    }
    #ifdef DEBUG
    printf("Innovations after mutation:\n");
    print_innovations(&pop->innovations);
    #endif
}

void population_kill_weak(population_t* pop, float* scores)
{
    #ifdef DEBUG
    printf("Killing weak agents...\n");
    #endif
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
            int total_params = pop->agents[i]->node_count + pop->agents[i]->weight_count;
            float net_size_cost = (float) -1 * (total_params * total_params);
            float total_cost = scores[i] + pop->network_size_cost_weight * net_size_cost;
            if(total_cost >= best_scores[j])
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
                best_scores[j] = total_cost;
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
    struct int_stack* stack = NULL;
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        a->activation_count[i] = 1;
        stack = int_stack_push(stack, i);
    }

    #ifdef DEBUG
    int propagation_count = 1;
    printf("Starting propagation 1, nodes... ");
    #endif

    // Forward through the network
    struct int_stack* next_stack = NULL;
    while (stack != NULL)
    {
        
        int node_id = stack->value;
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

        case 2:
            forward_act = leaky_relu(forward_act);
            break;
        
        case 3:
            forward_act = sigmoid(forward_act);
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
                next_stack = int_stack_push(next_stack, forward_node);
            }
        }

        struct int_stack* tmp = stack->next;  // TODO: should this be a stack_pop func?
        free(stack);
        stack = tmp;

        if (stack == NULL)
        {
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
    // TODO:
    for (size_t i = 0; i < pop->innovations.size; i++)
    {
        gene_free(*(gene_t*)vector_get(&pop->innovations, i));
    }
    free(pop);
}

void agent_free(agent_t* a)
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
    // TODO:
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
