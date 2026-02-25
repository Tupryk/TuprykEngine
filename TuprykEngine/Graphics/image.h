#ifndef GRAPHICS_IMAGE
#ifndef GRAPHICS_IMAGE

#include "../LinAlg/tensor.h"


struct image
{
    struct tensor* pixels;
};

struct image* new_image(int width, int height, int channels);
void image_set_pixel(struct image* im, int x, int y, float r, float g, float b, float a);
void image_free(struct image* im);

#endif
