#include "configuration.h"

int config_colliding(struct Config* C)
{
    return 0;
}

void config_free(struct Config* C)
{
    tensor_free(C->q);
    tensor_free(C->q_max);
    tensor_free(C->q_min);
    free(C);
}
