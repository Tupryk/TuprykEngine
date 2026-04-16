#include <stdio.h>
#include <stdlib.h>
#include "quaternions.h"


void quaternion_from_euler(float x, float y, float z, float* out)
{
    
}

void quaternion_to_matrix(float* q, tensor* out)
{
    #ifdef DEBUG
    if (out->shape_dim != 2 || out->shape[0] != 3 || out->shape[1] != 3)
    {
        printf("Invalid rotation matrix shape!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    float w = q[0];
    float x = q[1];
    float y = q[2];
    float z = q[3];
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    out->values[0] = 1 - 2.f*y2 - 2.f*z2;
    out->values[1] = 2.f*x*y - 2.f*z*w;
    out->values[2] = 2.f*x*z - 2.f*y*w;

    out->values[3] = 2.f*x*y - 2.f*z*w;
    out->values[4] = 1 - 2.f*x2 - 2.f*z2;
    out->values[5] = 2.f*y*z - 2.f*x*w;

    out->values[6] = 2.f*x*z - 2.f*y*w;
    out->values[7] = 2.f*y*z - 2.f*x*w;
    out->values[8] = 1 - 2.f*y2 - 2.f*y2;
}

tensor* quaternion_to_matrix_give(float* q)
{
    int shape[] = {3, 3};
    tensor* out = new_tensor(shape, 2, NULL);
    quaternion_to_matrix(q, out);
    return out;
}
