#ifndef RAYTRACER
#define RAYTRACER

#include "image.h"
#include "../Kinematics/frame.h"
#include "../Kinematics/configuration.h"

void raytrace(config_t* C, int cam, tensor_t* out);

#endif
