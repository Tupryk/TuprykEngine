#include "forces.h"

void center_of_mass(config* C, int root, tensor* com, float* total_mass);
{
    #ifdef DEBUG
    if (com->shape_dim != 2 || com->shape[0] != 3 || com->shape[1] != 1)
    {
        printf("Invalid COM vector for output!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    // TODO:
}

void centroidal_forces(config* C, int root, tensor* force, tensor* torque)
{
    // TODO:
}
