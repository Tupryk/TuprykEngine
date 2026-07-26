#ifndef RRT
#define RRT

#include "../../Algos/lists.h"
#include "../../LinAlg/tensor.h"
#include "../../Configuration/configuration.h"

typedef struct
{
    tensor_t* q;
    int parent;
} rrt_node;

stack* rrt_find_path(config* C, tensor_t* q0, tensor_t* qT, float stepsize);

#endif
