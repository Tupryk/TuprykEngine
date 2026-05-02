#include <stdio.h>
#include <stdlib.h>

#include "mlp.h"
#include "../Funcs/funcs.h"


mlp_t* mlp_init(int layer_count, int* layer_sizes, int* activation_types)
{
    mlp_t* mlp = (mlp_t*) malloc(sizeof(mlp_t));
    mlp->layer_count = layer_count;
    mlp->activation_types = (int*) malloc(sizeof(int) * layer_count);
    mlp->biases = (tensor**) malloc(sizeof(tensor*) * layer_count);
    mlp->weights = (tensor**) malloc(sizeof(tensor*) * (layer_count - 1));

    for (int i = 0; i < layer_count; i++)
    {
        mlp->activation_types[i] = activation_types[i];
        mlp->biases[i] = new_tensor_vector(layer_sizes[i], NULL);
        tensor_fill_uniform(mlp->biases[i], -1.f, 1.f);

        if (i != layer_count-1)
        {
            mlp->weights[i] = new_tensor_matrix(layer_sizes[i], layer_sizes[i+1], NULL);
            tensor_fill_uniform(mlp->weights[i], -1.f, 1.f);
        }
    }

    return mlp;
}

void mlp_forward(mlp_t* mlp, tensor* input, tensor* output)
{
    // tensor_apply_operation(..., relu);
}

void mlp_free(mlp_t* mlp)
{
    free(mlp->activation_types);
    for (int i = 0; i < mlp->layer_count-1; i++)
    {
        tensor_free(mlp->biases[i]);
        if (i != mlp->layer_count-1) tensor_free(mlp->weights[i]);
    }
    free(mlp->biases);
    free(mlp->weights);
    free(mlp);
}
