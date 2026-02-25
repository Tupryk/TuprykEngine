#ifndef RRT
#define RRT

#include "../../LinAlg/tensor.h"
#include "../../Configuration/configuration.h"

struct rrt_node
{
    int idx;
    struct tensor* q;
    struct rrt_node* parent;
    struct rrt_node* next;
};

struct tensor* rrt_find_path(struct config* C, struct tensor* q0, struct tensor* qT, float stepsize);

#endif
