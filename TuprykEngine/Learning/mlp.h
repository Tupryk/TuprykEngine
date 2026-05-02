#ifndef MLP
#define MLP

#include "../LinAlg/tensor.h"

typedef struct
{
    int layer_count;
    int* activation_types;  // TODO: should point to functions directly
    tensor** biases;
    tensor** weights;
} mlp_t;

mlp_t* mlp_init(int layer_count, int* layer_sizes, int* activation_types);
void mlp_forward(mlp_t* mlp, tensor* input, tensor* output);
void mlp_free(mlp_t* mlp);

#endif
