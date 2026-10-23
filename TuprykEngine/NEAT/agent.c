#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "agent.h"
#include "../Algos/lists.h"
#include "../Stochastic/sample.h"


agent_t* agent_new(int initial_weight_count)
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
    return new_agent;
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
        if (a->out_count[i] > 0)
        {
            free(a->out_id[i]);
            free(a->out_weight[i]);
        }
    }

    free(a->out_id);
    free(a->out_count);
    free(a->out_weight);
    
    free(a->in_count);
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

void reset_agent(agent_t* a)
{
    for (int i = 0; i < a->node_count; i++)
    {
        a->activations[i] = 0.f;
        a->activation_count[i] = 0;
    }
}

void agent_feed(agent_t* a, int in_dim, int out_dim, float* input, float* output)
{
    reset_agent(a);

    int in_counting[a->node_count];
    memset(in_counting, 0, sizeof(in_counting));

    // Initialize the input layer
    int_stack_t* stack = int_stack_init();
    for (int i = 0; i < in_dim; i++)
    {
        a->activations[i] = input[i];
        a->activation_count[i]++;
        int_stack_push(stack, i);
    }

    // Forward through the network
    int_stack_t* next_stack = int_stack_init();
    while (stack->size > 0)
    {
        int node_id = int_stack_pop(stack);
        // Pass through non-linearity
        float node_activation = a->activations[node_id];
        float forward_act = node_activation + a->activation_biases[node_id];
        int act_type = a->activation_funcs[node_id];
        
        #ifdef DEBUG
        if (act_type >= activation_count)
        {
            printf("Invalid node activation function!\n");
            exit(EXIT_FAILURE);
        }
        #endif
        
        forward_act = activations[act_type](forward_act);

        for (int i = 0; i < a->out_count[node_id]; i++)
        {
            int forward_node = a->out_id[node_id][i];
            float w = a->out_weight[node_id][i];
            a->activations[forward_node] += w * forward_act;
            in_counting[forward_node]++;
            if (
                in_counting[forward_node] == a->in_count[forward_node] &&
                !a->activation_count[forward_node]
            ) {
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
