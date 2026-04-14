#ifndef CONFIG_FRAME
#define CONFIG_FRAME

#include "../LinAlg/tensor.h"


typedef struct
{
    tensor* pos;
    tensor* rot;
    
    int* children;

    int type;
    void* data;
} frame;

void frame_free(frame* f);
void frame_get_pose_matrix(frame* f, tensor* pose);
tensor* frame_get_pose_matrix_give(frame* f);

#endif
