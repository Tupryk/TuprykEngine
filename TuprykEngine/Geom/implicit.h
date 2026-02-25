#ifndef IMPLICIT
#define IMPLICIT

#include "../LinAlg/tensor.h"

float implicit_sphere(struct tensor* x, struct tensor* pos, float r);
struct tensor* implicit_sphere_delta(struct tensor* x, struct tensor* pos, float r);
struct tensor* implicit_sphere_delta2(struct tensor* x, struct tensor* pos, float r);

// float implicit_plane();
// struct tensor* implicit_plane_delta();
// struct tensor* implicit_plane_delta2();

#endif
