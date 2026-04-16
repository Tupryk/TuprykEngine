#ifndef RAYTRACER
#define RAYTRACER

#include "image.h"
#include "../Kinematics/frame.h"
#include "../Kinematics/configuration.h"

void raytrace(config* C, int cam, tensor* out);

#endif
