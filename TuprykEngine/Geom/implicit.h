#ifndef IMPLICIT
#define IMPLICIT

#include "../LinAlg/tensor.h"

float implicit_sphere(tensor* x, tensor* pos, float r);
tensor* implicit_sphere_delta(tensor* x, tensor* pos, float r);
tensor* implicit_sphere_delta2(tensor* x, tensor* pos, float r);

// float implicit_plane();
// tensor* implicit_plane_delta();
// tensor* implicit_plane_delta2();

#endif
