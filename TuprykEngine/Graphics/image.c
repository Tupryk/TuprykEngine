#include "image.h"
#include "stdlib.h"


tensor_t* new_image(int width, int height, int channels)
{
    
    int shape[3] = {width, height, channels};
    tensor_t* im = new_tensor(shape, 3, NULL);
    return im;
}

void image_set_pixel(tensor_t* im, int x, int y, float r, float g, float b, float a)
{
    
}
