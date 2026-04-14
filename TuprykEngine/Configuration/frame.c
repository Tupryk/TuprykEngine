#include <stdlib.h>
#include <stdio.h>

#include "frame.h"
#include "../Geom/quaternions.h"


void frame_free(frame* f)
{
    tensor_free(f->pos);
    tensor_free(f->rot);
    free(f);
}

void frame_get_pose_matrix(frame* f, tensor* out)
{
    #ifdef DEBUG
    if (
        out->shape_dim != 2 || out->shape[0] != 4 || out->shape[1] != 4
    ) {
        printf("Pose matrix must be 4X4!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    float* pos = f->pos->values;
    out->values[3] = pos[0];
    out->values[7] = pos[1];
    out->values[11] = pos[2];
    
    // TODO: Use tensor slices here: tensor slices should be tensor structs but without allocated memory for the values

    out->values[12] = 0.f;
    out->values[13] = 0.f;
    out->values[14] = 0.f;
    out->values[15] = 1.f;
}

tensor* frame_get_pose_matrix_give(frame* f)
{
    int shape[] = {4, 4};
    tensor* out = new_tensor(shape, 2, NULL);
    frame_get_pose_matrix(f, out);
    return out;
}
