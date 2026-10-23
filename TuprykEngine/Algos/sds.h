#ifndef SDS
#define SDS

#include "lists.h"

struct grid_sds
{
    int grid_size;
    float grid_dim;
    int_stack_t** grid;
};

struct grid_sds* grid_sds_init(int grid_size, float grid_dim);
int_stack_t* grid_sds_query(struct grid_sds* sds, int id, float radius);
void grid_sds_free(struct grid_sds* sds);

#endif
