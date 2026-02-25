#include "image.h"
#include "stdlib.h"


struct image* new_image(int width, int height, int channels)
{
    struct image* im = (struct image*) malloc(sizeof(struct image));

    int shape[3] = {width, height, channels};
    im->pixels = new_tensor(shape, 3, NULL);
    
    return im;
}

void image_set_pixel(struct image* im, int x, int y, float r, float g, float b, float a)
{
    
}

void image_free(struct image* im)
{
    tensor_free(im->pixels);
    free(im);
}
