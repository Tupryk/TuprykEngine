#include "image.h"
#include "stdlib.h"


struct Image* new_image(int width, int height, int channels)
{
    struct Image* im = (struct Image*) malloc(sizeof(struct Image));

    int shape[3] = {width, height, channels};
    im->pixels = new_tensor(shape, 3, NULL);
    
    return im;
}

void image_set_pixel(struct Image* im, int x, int y, float r, float g, float b, float a)
{
    
}

void image_free(struct Image* im)
{
    tensor_free(im->pixels);
    free(im);
}
