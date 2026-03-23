#ifndef RRT
#define RRT

#include "../../LinAlg/tensor.h"
#include "../../Configuration/configuration.h"

struct rrt_node
{
    int idx;
    tensor* q;
    struct rrt_node* parent;
    struct rrt_node* next;
};

tensor* rrt_find_path(struct config* C, tensor* q0, tensor* qT, float stepsize);

#endif
