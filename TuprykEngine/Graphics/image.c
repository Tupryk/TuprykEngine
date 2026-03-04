#include "image.h"
#include "stdlib.h"


struct tensor* new_image(int width, int height, int channels)
{
    
    int shape[3] = {width, height, channels};
    struct tensor* im = new_tensor(shape, 3, NULL);
    return im;
}

void image_set_pixel(struct tensor* im, int x, int y, float r, float g, float b, float a)
{
    
}
