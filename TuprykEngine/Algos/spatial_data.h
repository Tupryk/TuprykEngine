#ifndef SPATIAL_DATA
#define SPATIAL_DATA

#include "../LinAlg/tensor.h"

typedef struct
{
    int dim;
    int size;
    tensor_t** points;
} kd_tree_t;

kd_tree_t* kd_tree_init(tensor_t** points, int count);
void kd_tree_knn(kd_tree_t* kt, tensor_t* query, int k, int* indices, float* dists);
// void kd_tree_in_sphere(tensor_t* pos, float r);
void kd_tree_free(kd_tree_t* kt);

#endif
