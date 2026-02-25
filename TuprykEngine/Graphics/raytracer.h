#ifndef RAYTRACER
#define RAYTRACER

#include "image.h"
#include "../Configuration/frame.h"
#include "../Configuration/configuration.h"

void raytrace(struct config* C, struct frame* cam, struct image* out);

#endif
