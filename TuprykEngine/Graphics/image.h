#ifndef GRAPHICS_IMAGE
#define GRAPHICS_IMAGE

#include "../LinAlg/tensor.h"


struct tensor* new_image(int width, int height, int channels);
void image_set_pixel(struct tensor* im, int x, int y, float r, float g, float b, float a);

#endif
