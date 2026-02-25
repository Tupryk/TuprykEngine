#include "rrt.h"
#include <float.h>


struct tensor* rrt_find_path(struct config* C, struct tensor* q0, struct tensor* qT, float stepsize)
{
    struct tensor* q_target = tensor_copy_shape(C->q);
    struct rrt_node* root = (struct rrt_node*) malloc(sizeof(struct rrt_node));
    struct tensor* diff = tensor_copy_shape(root->q);
    root->idx = 0;
    root->parent = NULL;
    root->q = tensor_copy(q0);
    root->next = NULL;

    struct rrt_node* latest_node = root;
    
    while (1)
    {
        int sample_goal = rand()%1000 * 0.0001 < 0.2;
        if (sample_goal) {
            tensor_transfer_values(q_target, qT);
        } else {
            tensor_uniform(C->q_min, C->q_max, q_target);
        }
    
        // TODO: kd_trees
        struct rrt_node* q_close = root;
        struct rrt_node* current_check = root;
        float min_dist = FLT_MAX;
        while (1)
        {
            tensor_sub(q_target, current_check->q, diff);
            float dist = vec_norm(diff);
            if (dist < min_dist)
            {
                q_close = current_check;
                min_dist = dist;
            }
            if (current_check->next != NULL) break;
            current_check = current_check->next;
        }
        
        tensor_sub(q_target, q_close, C->q);
        float dist = vec_norm(C->q);
        tensor_scalar_mult(C->q, stepsize);
        tensor_sub(C->q, q_close, C->q);
    
        if (!config_colliding(C))
        {
            // Expand
            struct rrt_node* new_node = (struct rrt_node*) malloc(sizeof(struct rrt_node));
            
            new_node->idx = latest_node->idx + 1;
            new_node->q = tensor_copy(qT);
            new_node->parent = q_close;
            new_node->next = NULL;

            latest_node->next = new_node;
            latest_node = new_node;
            
            if (sample_goal && dist <= stepsize) break;
        }
    }
    
    tensor_free(q_target);
    tensor_free(diff);

    // Build path
    
    return path;
}
