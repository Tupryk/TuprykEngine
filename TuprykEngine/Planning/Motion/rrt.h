#ifndef RRT
#define RRT

#include "../../Algos/lists.h"
#include "../../LinAlg/tensor.h"
#include "../../Configuration/configuration.h"

typedef struct
{
    tensor* q;
    int parent;
} rrt_node;

stack* rrt_find_path(config* C, tensor* q0, tensor* qT, float stepsize);

#endif
