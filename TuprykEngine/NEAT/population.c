#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "population.h"
#include "../Funcs/funcs.h"
#include "../Stochastic/sample.h"


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

population_t* init_population(int in_dim, int out_dim, int size)
{
    population_t* pop = (population_t*) malloc(sizeof(population_t));

    pop->in_dim = in_dim;
    pop->out_dim = out_dim;

    // TODO: neat_opt struct and default struct
    pop->size = size;
    pop->network_size_cost_weight = 0.f;
    pop->new_node_mutation_prob = 0.03f;
    pop->new_link_mutation_prob = 0.05f;  // Larger population can tolerate a larger number of prospective species -> 0.3f
    pop->weights_mutation_prob = 0.8f;
    pop->weights_mutation_prob_resample = 0.1f;
    pop->speciation_thresh = 3.f;
    pop->crossover_offspring_prob = 0.25f;

    pop->agent_to_species = malloc(sizeof(int) * pop->size);
    memset(pop->agent_to_species, -1, sizeof(int) * pop->size);
    pop->species = vector_init(sizeof(agent_t*));
        
    pop->innovations = vector_init(sizeof(gene_t));
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

    pop->agents = (agent_t**) malloc(sizeof(agent_t*) * pop->size);
    for (int i = 0; i < pop->size; i++) pop->agents[i] = NULL;
    
    int initial_weight_count = in_dim * out_dim;

    for (int i = 0; i < pop->size; i++)
    {
        agent_t* new_agent = agent_new(initial_weight_count);

        agent_maybe_mutate(pop, new_agent);

        agent_build_network(pop, new_agent);
        population_add_agent(pop, new_agent, i);
    }

    return pop;
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

void population_resample(population_t* pop, float* fitness)  // TODO: This might be a bit expensive...
{
    
    // Zero mean fitness
    float offset_fitness[pop->size];
    float min_fitness = fitness[0];
    for (int i = 1; i < pop->size; i++) if (min_fitness > fitness[i]) min_fitness = fitness[i];
    for (int i = 0; i < pop->size; i++) offset_fitness[i] = fitness[i] - min_fitness;
    
    // Compute adjusted fitness
    size_t prev_gen_species_count = pop->species.size;
    float adjusted_fitness[pop->size];
    
    float avg_adjusted_fitness[prev_gen_species_count];
    memset(avg_adjusted_fitness, 0.f, sizeof(avg_adjusted_fitness));
    int species_size[prev_gen_species_count];
    memset(species_size, 0, sizeof(species_size));
    int species_agent_indices[pop->size][pop->size];

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
        
        // TODO: Where should network_size_cost go?
        float network_size_cost = ((float) pop->agents[i]->node_count);
        network_size_cost *= network_size_cost;
        network_size_cost *= pop->network_size_cost_weight;
        adjusted_fitness[i] = offset_fitness[i] / sum_of_fellows - network_size_cost;
        
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
            if (adjusted_fitness[ai] > species_champions_score[i] || species_champions[i] == NULL)
            {
                species_champions_score[i] = adjusted_fitness[ai];
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
                parent_a_idx = species_agent_indices[i][parent_a_idx];
                parent_b_idx = species_agent_indices[i][parent_b_idx];

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
        printf("Desired population size: %d; Last child index: %d\n", pop->size, agent_index);
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
    
    a->out_count = (int*) malloc(sizeof(int) * node_count);
    a->out_id = (int**) malloc(sizeof(int*) * node_count);
    a->out_weight = (float**) malloc(sizeof(float*) * node_count);
    
    a->in_count = (int*) malloc(sizeof(int) * node_count);
    
    int connection_counter[node_count];
    int initial_node_count = pop->in_dim + pop->out_dim;
    for (int i = 0; i < node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
        if (i < initial_node_count) a->activation_funcs[i] = 0;
        else a->activation_funcs[i] = 2;
        a->activation_biases[i] = 0.f;
        a->out_count[i] = 0;
        a->in_count[i] = 0;
        connection_counter[i] = 0;
    }

    for (int i = 0; i < a->gene_count; i++)
    {
        if (a->gene_enabled[i])
        {
            gene_t gene = *(gene_t*)vector_get(&pop->innovations, a->genes[i]);
            a->out_count[pop_to_agent[gene.out]]++;
            a->in_count[pop_to_agent[gene.in]]++;
        }
    }

    for (int i = 0; i < node_count; i++)
    {
        a->out_id[i] = (int*) malloc(sizeof(int) * a->out_count[i]);
        a->out_weight[i] = (float*) malloc(sizeof(float) * a->out_count[i]);
    }

    for (int i = 0; i < a->gene_count; i++)
    {
        if (a->gene_enabled[i])
        {
            gene_t gene = *(gene_t*) vector_get(&pop->innovations, a->genes[i]);
    
            int out = pop_to_agent[gene.out];
            int conn_id = connection_counter[out];
            a->out_id[out][conn_id] = pop_to_agent[gene.in];
            a->out_weight[out][conn_id] = a->gene_weights[i];
    
            connection_counter[out]++;
        }
    }
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
