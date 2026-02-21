#ifndef GRAPHICS_IMAGE
#ifndef GRAPHICS_IMAGE

#include "../LinAlg/tensor.h"


struct Image
{
    struct tensor* pixels;
};

struct Image* new_image(int width, int height, int channels);
void image_set_pixel(struct Image* im, int x, int y, float r, float g, float b, float a);
void image_free(struct Image* im);

#endif
