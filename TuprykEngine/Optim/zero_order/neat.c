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

agent_t* agent_copy_genome(agent_t* a)
{
    agent_t* a_copy = (agent_t*) malloc(sizeof(agent_t));
    a_copy->gene_count = a->gene_count;
    
    int genes_int_size = sizeof(int) * a->gene_count;
    int genes_float_size = sizeof(float) * a->gene_count;

    a_copy->genes = (int*) malloc(genes_int_size);
    memcpy(a_copy->genes, a->genes, genes_int_size);

    a_copy->gene_enabled = (int*) malloc(genes_int_size);
    memcpy(a_copy->gene_enabled, a->gene_enabled, genes_int_size);
 
    a_copy->gene_weights = (float*) malloc(genes_float_size);
    memcpy(a_copy->gene_weights, a->gene_weights, genes_float_size);

    return a_copy;
}

void agent_build_network(population_t* pop, agent_t* a)
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
        if (a->gene_enabled[i])
        {
            gene_t gene = *(gene_t*)vector_get(&pop->innovations, a->genes[i]);
            a->connection_counts[pop_to_agent[gene.out]]++;
        }
    }

    for (int i = 0; i < node_count; i++)
    {
        a->connections[i] = (int*) malloc(sizeof(int) * a->connection_counts[i]);
        a->connection_weight[i] = (float*) malloc(sizeof(float) * a->connection_counts[i]);
    }

    for (int i = 0; i < a->gene_count; i++)
    {
        if (a->gene_enabled[i])
        {
            gene_t gene = *(gene_t*) vector_get(&pop->innovations, a->genes[i]);
    
            int out = pop_to_agent[gene.out];
            int conn_id = connection_counter[out];
            a->connections[out][conn_id] = pop_to_agent[gene.in];
            a->connection_weight[out][conn_id] = a->gene_weights[i];
    
            connection_counter[out]++;
        }
    }
}

void population_add_agent(population_t* pop, agent_t* new_agent, int at_index)
{
    if (pop->agents[at_index] != NULL)
    {
        // TODO: Some other cleaning maybe?
        agent_free(pop->agents[at_index]);
    }

    pop->agents[at_index] = new_agent;

    // Assign to a species
    for (int i = pop->species.size-1; i >= 0; i--)
    {
        agent_t* a = *(agent_t**) vector_get(&pop->species, i);
        if (agent_compatibility_dist(a, new_agent) < pop->speciation_thresh)
        {
            pop->agent_to_species[at_index] = i;
            return;
        }
    }

    // New species!!!
    pop->agent_to_species[at_index] = pop->species.size;
    agent_t* agent_genome_copy = agent_copy_genome(new_agent);
    vector_push(&pop->species, &agent_genome_copy);
}

population_t* init_population(int in_dim, int out_dim)
{
    population_t* pop = (population_t*) malloc(sizeof(population_t));
    
    pop->in_dim = in_dim;
    pop->out_dim = out_dim;

    // TODO: neat_opt struct and default struct
    pop->size = 150;
    pop->network_size_cost_weight = 10.f;
    pop->new_node_mutation_prob = 0.03f;
    pop->new_link_mutation_prob = 0.05f;  // Larger population can tolerate a larger number of prospective species -> 0.3f
    pop->weights_mutation_prob = 0.8f;
    pop->weights_mutation_prob_resample = 0.1f;
    pop->speciation_thresh = 3.f;
    pop->crossover_offspring_prob = 0.25f;

    pop->agent_to_species = malloc(sizeof(int) * pop->size);
    memset(pop->agent_to_species, -1, sizeof(int) * pop->size);
    pop->species = vector_create(sizeof(agent_t*));
        
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

    pop->agents = (agent_t**) malloc(sizeof(agent_t*) * pop->size);
    for (int i = 0; i < pop->size; i++) pop->agents[i] = NULL;
    
    int initial_weight_count = in_dim * out_dim;

    for (int i = 0; i < pop->size; i++)
    {
        agent_t* new_agent = (agent_t*) malloc(sizeof(agent_t));
        
        new_agent->gene_count = initial_weight_count;
        new_agent->genes = (int*) malloc(sizeof(int) * initial_weight_count);
        new_agent->gene_enabled = (int*) malloc(sizeof(int) * initial_weight_count);
        new_agent->gene_weights = (float*) malloc(sizeof(float) * initial_weight_count);
        for (int j = 0; j < initial_weight_count; j++)
        {
            new_agent->genes[j] = j;
            new_agent->gene_enabled[j] = 1;
            new_agent->gene_weights[j] = rand_uni(-1.f, 1.f);
        }

        agent_maybe_mutate(pop, new_agent);

        agent_build_network(pop, new_agent);
        population_add_agent(pop, new_agent, i);
    }

    return pop;
}

float agent_compatibility_dist(agent_t* agent_a, agent_t* agent_b)
{
    float excess_weight = 1.f;
    float disjoint_weight = 1.f;
    float weight_diff_weight = 0.4f;

    // Agent-A always has the excess genes
    if (agent_a->genes[agent_a->gene_count-1] < agent_b->genes[agent_b->gene_count-1])
    {
        agent_t* tmp = agent_a;
        agent_a = agent_b;
        agent_b = tmp;
    }
    int larger_genome = agent_a->gene_count > agent_b->gene_count ? agent_a->gene_count : agent_b->gene_count;

    int excess_count = 0;
    int disjoint_count = 0;

    int matching_genes = 0;
    float avg_weight_diff = 0.f;

    int a_gene_id = 0;
    int b_gene_id = 0;
    
    while (1)
    {
        if (b_gene_id >= agent_b->gene_count)
        {
            excess_count++;
            a_gene_id++;
            if (a_gene_id >= agent_a->gene_count) break;
        }
        else if (agent_a->genes[a_gene_id] == agent_b->genes[b_gene_id])
        {
            avg_weight_diff += fabsf(agent_a->gene_weights[a_gene_id] - agent_b->gene_weights[b_gene_id]);
            matching_genes++;

            a_gene_id++;
            b_gene_id++;
        }
        else
        {
            disjoint_count++;
            if (agent_a->genes[a_gene_id] > agent_b->genes[b_gene_id]) b_gene_id++;
            else a_gene_id++;
        }
    }
    avg_weight_diff /= (float) matching_genes;

    float dist = (
        ((float) excess_count) / ((float) larger_genome) * excess_weight +
        ((float) disjoint_count) / ((float) larger_genome) * disjoint_weight +
        avg_weight_diff * weight_diff_weight
    );
    return dist;
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

void agent_insert_gene(agent_t* a, int gene_id)
{
    a->gene_count++;
    int gene_count = a->gene_count;

    a->genes = realloc(a->genes, sizeof(int) * gene_count);
    a->gene_enabled = realloc(a->gene_enabled, sizeof(int) * gene_count);
    a->gene_weights = realloc(a->gene_weights, sizeof(float) * gene_count);

    int shift_id = a->gene_count-1;
    while (1)
    {
        if (shift_id <= 0) break;
        if (a->genes[shift_id-1] < gene_id) break;
        shift_id--;
    }

    int prev_id = gene_id;
    int prev_enabled = 1;
    float prev_weight = rand_uni(-1.f, 1.f);
    for (int i = shift_id; i < gene_count; i++)
    {
        int tmp_i = a->genes[i];
        a->genes[i] = prev_id;
        prev_id = tmp_i;

        int tmp_e = a->gene_enabled[i];
        a->gene_enabled[i] = prev_enabled;
        prev_enabled = tmp_e;

        float tmp_w = a->gene_weights[i];
        a->gene_weights[i] = prev_weight;
        prev_weight = tmp_w;
    }
}

void agent_maybe_mutate(population_t* pop, agent_t* a)
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

    int mutate_link = pop->new_link_mutation_prob > rand_uni(0.f, 1.f);
    int mutate_node = pop->new_node_mutation_prob > rand_uni(0.f, 1.f);
    int mutate_weights = pop->weights_mutation_prob > rand_uni(0.f, 1.f);

    gene_t link_mutation;
    if (mutate_link)
    {
        int mutating = 1;
        while (mutating)
        {
            mutating = 0;

            int out_node = rand() % node_count;
            int in_node = rand() % (node_count-1);
            if (in_node >= out_node) in_node++;

            link_mutation.out = node_numbers[out_node];
            link_mutation.in = node_numbers[in_node];

            for (int i = 0; i < gene_count; i++)
            {
                gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[i]);
                if (gene.in == link_mutation.in && gene.out == link_mutation.out)
                {
                    mutating = 1;
                    break;
                }
            }
        }
    }

    gene_t node_mutation1;
    gene_t node_mutation2;
    if (mutate_node)
    {
        int gene_id = rand() % gene_count;
        gene_t gene = *(gene_t*) vector_get(&pop->innovations, genes[gene_id]);

        a->gene_enabled[gene_id] = 0;

        node_mutation1.out = gene.out;
        node_mutation1.in = newest_node_in_sequence;

        node_mutation2.out = newest_node_in_sequence;
        node_mutation2.in = gene.in;
    }

    if (mutate_weights)
    {
        for (int i = 0; i < gene_count; i++)
        {
            if (pop->weights_mutation_prob_resample > rand_uni(0.f, 1.f))
            {
                a->gene_weights[i] = rand_uni(-1.f, 1.f);
            }
            else
            {
                a->gene_weights[i] += rand_uni(-0.1f, 0.1f);
            }
        }
    }

    // Overwrite genes
    if (mutate_link)
    {
        agent_insert_gene(a, population_maybe_add_gene(pop, &link_mutation));
    }

    if (mutate_node)
    {
        agent_insert_gene(a, population_maybe_add_gene(pop, &node_mutation1));
        agent_insert_gene(a, population_maybe_add_gene(pop, &node_mutation2));
    }
}

void agent_feed(agent_t* a, int in_dim, int out_dim, float* input, float* output)
{
    // TODO: This is wrong not all inputs are fed into the neurons
    reset_agent(a);

    // Initialize the input layer
    int_stack* stack = int_stack_init();
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        a->activation_count[i]++;
        int_stack_push(stack, i);
    }

    // Forward through the network
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

        for (int i = 0; i < a->connection_counts[node_id]; i++)
        {
            int forward_node = a->connections[node_id][i];
            float w = a->connection_weight[node_id][i];
            a->activations[forward_node] += w * forward_act;
            if (!a->activation_count[forward_node])
            {
                a->activation_count[forward_node]++;
                int_stack_push(next_stack, forward_node);
            }
        }

        if (stack->size <= 0)
        {
            int_stack_free(stack);
            stack = next_stack;
            next_stack = int_stack_init();
        }
    }
    int_stack_free(stack);
    int_stack_free(next_stack);

    // Write to output array
    for (int i = 0; i < out_dim; i++)
    {
        output[i] = a->activations[in_dim+i];
    }
}

void population_feed_all_agents_same(population_t* pop, float* input, float** output)
{
    for (int i = 0; i < pop->size; i++)
    {
        agent_feed(pop->agents[i], pop->in_dim, pop->out_dim, input, output[i]);
    }
}

void population_feed_all_agents(population_t* pop, float** input, float** output)
{
    for (int i = 0; i < pop->size; i++)
    {
        agent_feed(pop->agents[i], pop->in_dim, pop->out_dim, input[i], output[i]);
    }
}

agent_t* agents_cross(agent_t* parent_a, agent_t* parent_b)
{
    // This function assumes that parent_a is more fit than parent_b.
    agent_t* child = agent_copy_genome(parent_a);

    int a_gene_id = 0;
    int b_gene_id = 0;
    
    while (a_gene_id < parent_a->gene_count)
    {
        if (b_gene_id >= parent_b->gene_count)
        {
            child->gene_weights[a_gene_id] = parent_a->gene_weights[a_gene_id];
            child->gene_enabled[a_gene_id] = parent_a->gene_enabled[a_gene_id];
            a_gene_id++;
        }
        else if (parent_a->genes[a_gene_id] == parent_b->genes[b_gene_id])
        {
            if (rand() % 2)
            {
                child->gene_weights[a_gene_id] = parent_a->gene_weights[a_gene_id];
                child->gene_enabled[a_gene_id] = parent_a->gene_enabled[a_gene_id];
            }
            else
            {
                child->gene_weights[a_gene_id] = parent_b->gene_weights[b_gene_id];
                child->gene_enabled[a_gene_id] = parent_b->gene_enabled[b_gene_id];
            }
            a_gene_id++;
            b_gene_id++;
        }
        else if (parent_a->genes[a_gene_id] < parent_b->genes[b_gene_id])
        {
            a_gene_id++;
        }
        else
        {
            b_gene_id++;
        }
    }

    return child;
}

void population_resample(population_t* pop, float* fitness)  // TODO: This might be a bit expensive...
{
    size_t prev_gen_species_count = pop->species.size;
    float adjusted_fitness[pop->size];
    float offset_fitness[pop->size];
    float min_fitness = fitness[0];
    for (int i = 1; i < pop->size; i++) if (min_fitness > fitness[i]) min_fitness = fitness[i];
    for (int i = 0; i < pop->size; i++) offset_fitness[i] = fitness[i] - min_fitness;
    
    float avg_adjusted_fitness[prev_gen_species_count];
    int species_size[prev_gen_species_count];
    memset(species_size, 0, sizeof(species_size));
    int species_agent_indices[pop->size][pop->size];

    // Compute adjusted fitness
    for (int i = 0; i < pop->size; i++)
    {
        int current_species = pop->agent_to_species[i];
        float sum_of_fellows = 1.f;
        
        for (int j = 0; j < pop->size; j++)
        {
            if (i == j) continue;
            if (current_species != pop->agent_to_species[j]) continue;
            sum_of_fellows += agent_compatibility_dist(pop->agents[i], pop->agents[j]);
        }
        
        adjusted_fitness[i] = offset_fitness[i] / sum_of_fellows + ((float) pop->agents[i]->node_count) * pop->network_size_cost_weight;
        
        species_agent_indices[current_species][species_size[current_species]] = i;
        species_size[current_species]++;
        avg_adjusted_fitness[current_species] += adjusted_fitness[i];
    }

    // Compute total adjusted fitness
    float total_adjusted_fitness = 0.f;
    for (size_t i = 0; i < prev_gen_species_count; i++)
    {
        if (!species_size[i]) continue;
        avg_adjusted_fitness[i] /= ((float) species_size[i]);
        total_adjusted_fitness += avg_adjusted_fitness[i];
    }

    // Allocate species budgets
    agent_t* species_champions[prev_gen_species_count];
    for (size_t i = 0; i < prev_gen_species_count; i++) species_champions[i] = NULL;
    float species_champions_score[prev_gen_species_count];
    int species_budget[prev_gen_species_count];
    memset(species_budget, 0, sizeof(species_budget));
    int total_budget = 0;
    for (size_t i = 0; i < prev_gen_species_count; i++)
    {
        if (!species_size[i]) continue;
        
        species_budget[i] = avg_adjusted_fitness[i] / total_adjusted_fitness * ((float) pop->size);
        total_budget += species_budget[i];
        
        for (int j = 0; j < species_size[i]; j++)
        {
            int ai = species_agent_indices[i][j];
            if (avg_adjusted_fitness[ai] > species_champions_score[i] || species_champions[i] == NULL)
            {
                species_champions_score[i] = avg_adjusted_fitness[ai];
                species_champions[i] = pop->agents[ai];
            }
        }
    }
    int budget_remainder = pop->size - total_budget;
    size_t species_id = 0;
    for (int i = 0; i < budget_remainder; i++)
    {
        if (!species_size[species_id])
        {
            i--;
            species_id++;
            continue;
        }
        species_budget[species_id]++;
        species_id++;
        if (species_id >= prev_gen_species_count) species_id = 0;
    }

    // Create new generation
    agent_t* prev_gen[pop->size];
    for (int i = 0; i < pop->size; i++)
    {
        prev_gen[i] = pop->agents[i];
        pop->agents[i] = NULL;
    }

    int agent_index = 0;
    for (size_t i = 0; i < prev_gen_species_count; i++)
    {
        int s_size = species_size[i];
        if (!s_size) continue;

        // TODO: Ignore worst 50%
        float agent_probs[s_size];
        float species_scores[s_size];
        float total_score = 0.f;
        for (int j = 0; j < s_size; j++)
        {
            float score = offset_fitness[species_agent_indices[i][j]];
            agent_probs[j] = score;
            species_scores[j] = score;
            total_score += score;
        }
        if (fabsf(total_score) < 1e-6)
        {
            for (int j = 0; j < s_size; j++) agent_probs[j] = 1.f / (float) s_size;
        }
        else
        {
            for (int j = 0; j < s_size; j++)
            {
                agent_probs[j] = agent_probs[j] / total_score;
            }
        }
        
        for (int j = 0; j < species_budget[i]; j++)
        {
            agent_t* child;
            if (j == 0)
            {
                // Keep champion
                child = agent_copy_genome(species_champions[i]);
                
                // Overwrite species representative
                agent_t* prev_species_representative = *(agent_t**) vector_get(&pop->species, i);
                agent_free(prev_species_representative);

                // TODO: Don't overwrite current species vector
                agent_t* new_species_representative = agent_copy_genome(species_champions[i]);
                vector_set(&pop->species, i, &new_species_representative);
            }
            else if (pop->crossover_offspring_prob > rand_uni(0.f, 1.f) && s_size > 1)
            {
                // Crossover
                int parent_a_idx = sample_weighted_elems(agent_probs, s_size);
                int parent_b_idx = sample_weighted_elems(agent_probs, s_size);
                while (parent_a_idx == parent_b_idx)
                {
                    parent_b_idx = sample_weighted_elems(agent_probs, s_size);
                }

                if (offset_fitness[parent_b_idx] > offset_fitness[parent_a_idx])
                {
                    int tmp = parent_a_idx;
                    parent_a_idx = parent_b_idx;
                    parent_b_idx = tmp;
                }

                agent_t* parent_a = prev_gen[parent_a_idx];  // Better parent
                agent_t* parent_b = prev_gen[parent_b_idx];
                
                child = agents_cross(parent_a, parent_b);
            }
            else
            {
                // Mutation
                int parent_idx = sample_weighted_elems(agent_probs, s_size);
                agent_t* parent = prev_gen[parent_idx];
                child = agent_copy_genome(parent);
                
                agent_maybe_mutate(pop, child);
            }
            agent_build_network(pop, child);
            population_add_agent(pop, child, agent_index);
            agent_index++;
        }
    }

    #ifdef DEBUG
    if (agent_index != pop->size)
    {
        printf("Something went wrong when generating offspring!\n");
        printf("Desired population size: %d; Last child index: %d\n", agent_index, pop->size);
        exit(EXIT_FAILURE);
    }
    #endif

    // Free old population
    for (int i = 0; i < pop->size; i++)
    {
        if (prev_gen[i] != NULL)
        {
            agent_free(prev_gen[i]);
        }
    }
}

void population_free(population_t* pop)
{
    for (int i = 0; i < pop->size; i++)
    {
        if (pop->agents[i] != NULL)
        {
            agent_free(pop->agents[i]);
        }
    }
    vector_free(&pop->innovations);
    for (size_t i = 0; i < pop->species.size; i++)
    {
        agent_t* a = *(agent_t**) vector_get(&pop->species, i);
        agent_free(a);
    }
    vector_free(&pop->species);
    free(pop->agent_to_species);
    free(pop);
}

void agent_free_network(agent_t* a)
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
        agent_free_network(a);
    }
    free(a->genes);
    free(a->gene_enabled);
    free(a->gene_weights);
    free(a);
}
