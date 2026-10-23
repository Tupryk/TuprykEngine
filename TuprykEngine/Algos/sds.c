#include "sds.h"


struct grid_sds* grid_sds_init(int grid_size, float grid_dim)
{
    struct grid_sds* sds = (struct grid_sds*) malloc(sizeof(struct grid_sds));

    sds->grid_size = grid_size;
    sds->grid_dim = grid_dim;
    int grid_dim3 = grid_size * grid_size * grid_size;

    sds->grid = (int_stack_t**) malloc(
        sizeof(int_stack_t*) * grid_dim3
    );
    for (int i = 0; i < grid_dim3; i++) sds->grid[i] = int_stack_init();

    return sds;
}

// TODO: Do a naive query to check if there is a performance gain.
int_stack_t* grid_sds_query(struct grid_sds* sds, int id, float radius)
{
    int_stack_t* query = int_stack_init();
    return query;
}

void grid_sds_free(struct grid_sds* sds)
{
    int grid_dim3 = sds->grid_size * sds->grid_size * sds->grid_size;
    for (int i = 0; i < grid_dim3; i++) int_stack_free(sds->grid[i]);
    free(sds);
}
