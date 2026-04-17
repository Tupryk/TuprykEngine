#include <stdlib.h>
#include <stdio.h>

#include "frame.h"
#include "../Geom/quaternions.h"


frame* frame_init(float* pos, float* rot)
{
    frame* f = (frame*) malloc(sizeof(frame));

    int pos_shape[] = {3, 1};
    f->pos = new_tensor(pos_shape, 2, pos);

    int rot_shape[] = {4, 1};
    f->rot = new_tensor(rot_shape, 2, rot);

    f->children_count = 0;
    f->children = NULL;

    f->data = NULL;

    f->parent = 0;
    f->type = 0;

    return f;
}

void frame_free(frame* f)
{
    tensor_free(f->pos);
    tensor_free(f->rot);

    if (f->type == 1)
    {
        geom* g = (geom*) f->data;
        if (g->type == 0)
        {
            free(g->mesh);
        }
        #ifdef DEBUG
        else
        {
            printf("Undefined mesh type %d!\n", g->type);
            exit(EXIT_FAILURE);
        }
        #endif
        free(g->tex);
        free(g);
    }
    else if (f->type == 2 || f->type == 3)
    {
        free(f->data);
    }
    #ifdef DEBUG
    else if (f->type != 0)
    {
        printf("Undefined frame type %d!\n", f->type);
        exit(EXIT_FAILURE);
    }
    #endif

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
