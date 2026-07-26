#ifndef IMPLICIT
#define IMPLICIT

#include "../LinAlg/tensor.h"

float implicit_sphere(tensor_t* x, tensor_t* pos, float r);
tensor_t* implicit_sphere_delta(tensor_t* x, tensor_t* pos, float r);
tensor_t* implicit_sphere_delta2(tensor_t* x, tensor_t* pos, float r);

// float implicit_plane();
// tensor_t* implicit_plane_delta();
// tensor_t* implicit_plane_delta2();

#endif
