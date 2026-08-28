#ifndef CAMERA
#define CAMERA

#include "../LinAlg/tensor.h"

typedef struct
{
    tensor_t* pos;
    tensor_t* rot;
    tensor_t* mat;
} camera_t;

#endif
