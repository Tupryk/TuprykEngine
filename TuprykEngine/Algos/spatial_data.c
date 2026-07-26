#include <stdio.h>
#include <stdlib.h>

#include "spatial_data.h"

// TODO:

kd_tree_t* kd_tree_init(tensor_t** points, int count)
{
    kd_tree_t* kt = (kd_tree_t*) malloc(sizeof(kd_tree_t));
    kt->dim = points[0]->volume;
    kt->size = count;
    kt->points = (tensor_t**) malloc(sizeof(tensor_t*) * count);

    for (int i = 0; i < count; i++) kt->points[i] = tensor_copy(points[i]);

    return kt;
}

void kd_tree_knn(kd_tree_t* kt, tensor_t* query, int k, int* indices, float* dists)
{
    tensor_t* diff = tensor_copy_shape(query);
    for (int i = 0; i < k; i++) indices[i] = -1;
    for (int i = 0; i < kt->size; i++)
    {
        tensor_sub(query, kt->points[i], diff);
        float dist = vector_squared_norm(diff);

        for (int j = 0; j < k; j++)
        {
            if (indices[j] == -1)
            {
                indices[j] = i;
                dists[j] = dist;
                break;
            }
            else if (dist < dists[j])
            {
                int new_idx = i;
                float new_dist = dist;
                for (int jt = j; jt < k; jt++)
                {
                    int tmp_idx = indices[jt];
                    float tmp_dist = dists[jt];
                    
                    indices[jt] = new_idx;
                    dists[jt] = new_dist;
                    
                    new_idx = tmp_idx;
                    new_dist = tmp_dist;
                    if (new_idx == -1) break;
                }
                break;
            }
        }
    }
    tensor_free(diff);
}

void kd_tree_free(kd_tree_t* kt)
{
    for (int i = 0; i < kt->size; i++) tensor_free(kt->points[i]);
    free(kt->points);
    free(kt);
}
