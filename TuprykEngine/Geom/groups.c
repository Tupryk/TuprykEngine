#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "groups.h"
#include "../LinAlg/tensor.h"


struct tensor make_se_mat(double pos[], double angles[], int dim)
{
    if (dim != 3)
    {
        printf("Special orthogonal group was only implemented for dim 3!\n");
        exit(EXIT_FAILURE);
    }

    int shape[] = {3, 3};
    double values[] = {
        1.0, 0.0, 0.0, pos[0],
        0.0, 1.0, 0.0, pos[1],
        0.0, 0.0, 1.0, pos[2],
        0.0, 0.0, 0.0, 1.0
    };
    struct tensor out = new_tensor(shape, 2, values);

    return out;
}
