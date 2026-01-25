#include <stdlib.h>
#include <stdio.h>
#include "meta.h"
#include "../LinAlg/tensor.h"


struct optim_logs* new_optim_logs()
{
    struct optim_logs* ol = (struct optim_logs*) malloc( sizeof(struct optim_logs) );
    
    ol->steps = 0;
    ol->Xs = NULL;
    ol->costs = NULL;
    ol->final_x = NULL;
    
    return ol;
}

void optim_logs_free(struct optim_logs* ol)
{
    if (ol->costs != NULL)
    {
        tensor_free(ol->costs);
        tensor_free(ol->Xs);
    }

    if (ol->final_x != NULL)
    {
        tensor_free(ol->final_x);
    }

    free(ol);
}

void optim_logs_add(struct optim_logs* ol, struct tensor* x, float cost)
{
    int cost_shape[] = {1, 1};
    float cost_value[] = {cost};
    struct tensor* new_cost = new_tensor(cost_shape, 2, cost_value);

    ol->steps++;
    if (ol->costs == NULL)
    {
        ol->costs = tensor_copy(new_cost);
        ol->Xs = tensor_copy(x);
    }
    else
    {
        struct tensor* new_Xs = tensor_append(ol->Xs, x, 1);
        tensor_free(ol->Xs);
        ol->Xs = new_Xs;
        struct tensor* new_costs = tensor_append(ol->costs, new_cost, 1);
        tensor_free(ol->costs);
        ol->costs = new_costs;
    }

    tensor_free(new_cost);
}
