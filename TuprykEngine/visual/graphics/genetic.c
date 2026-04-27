#include "genetic.h"
#include "window.h"
#include "basic.h"
#include "../prints/algos.h"


void draw_network(population_t* pop, agent_t* a)
{
    init_window();

    float node_pos[a->node_count][2];

    int activation_count[a->node_count];
    for (int i = 0; i < a->node_count; i++)
    {
        activation_count[i] = 0;
    }

    // Initialize the input layer
    int_stack* stack = int_stack_init();
    for (int i = 0; i < pop->in_dim; i++)
    {
        activation_count[i]++;
        int_stack_push(stack, i);
    }

    // Forward through the network
    int_stack* next_stack = int_stack_init();
    int layer_idx = 0;
    int in_layer_idx = 0;
    int layer_widths[a->node_count];
    int max_layer_width = 0;
    while (stack->size > 0)
    {
        int node_id = int_stack_pop(stack);

        node_pos[node_id][0] = (float) in_layer_idx;
        node_pos[node_id][1] = (float) layer_idx;
        in_layer_idx++;
        
        for (int i = 0; i < a->connection_counts[node_id]; i++)
        {
            int forward_node = a->connections[node_id][i];
            if (!activation_count[forward_node] && !int_stack_contains(next_stack, forward_node))
            {
                int_stack_push(next_stack, forward_node);
            }
        }
        
        if (stack->size <= 0)
        {
            int_stack_free(stack);
            stack = next_stack;
            next_stack = int_stack_init();

            layer_widths[layer_idx] = in_layer_idx;
            if (max_layer_width < in_layer_idx) max_layer_width = in_layer_idx;
            in_layer_idx = 0;
            layer_idx++;
        }
    }
    int_stack_free(next_stack);
    
    int layer_count = layer_idx;

    for (int i = 0; i < a->node_count; i++)
    {
        in_layer_idx = (int) node_pos[i][0];
        layer_idx = (int) node_pos[i][1];
        
        float total_width = ((float) (layer_widths[layer_idx]-1)) / ((float) (max_layer_width-1));
        float offset = (1.f - total_width * 0.8f) * 0.5f;
        float x = node_pos[i][0] / ((float) max_layer_width-1) * 0.8f + offset;
        float y = 1.f - (node_pos[i][1] / ((float) layer_count-1) * 0.8f + 0.1f);

        node_pos[i][0] = x;
        node_pos[i][1] = y;
    }

    set_color(1.f, 0.f, 0.f);
    for (int i = 0; i < a->node_count; i++)
    {
        for (int j = 0; j < a->connection_counts[i]; j++)
        {
            int k = a->connections[i][j];
            draw_line(
                node_pos[i][0], node_pos[i][1],
                node_pos[k][0], node_pos[k][1]
            );
        }
    }

    // Draw
    set_color(1.f, 1.f, 1.f);
    for (int i = 0; i < a->node_count; i++)
    {
        draw_circle(node_pos[i][0] * WINDOW_W, node_pos[i][1] * WINDOW_H, 0.025f * WINDOW_W);
    }
    
    window_wait();
}
