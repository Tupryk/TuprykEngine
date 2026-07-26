#ifndef GRAPHICS_IMAGE
#define GRAPHICS_IMAGE

#include "../LinAlg/tensor.h"


tensor_t* new_image(int width, int height, int channels);
void image_set_pixel(tensor_t* im, int x, int y, float r, float g, float b, float a);

#endif
