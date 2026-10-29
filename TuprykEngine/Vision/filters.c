#include "filters.h"


tensor_t* sobel_x_3d()
{
    int kernel_shape[] = {3, 3, 3};
    float kernel_values[] = {
        -1.f, 0.f, 1.f,
        -2.f, 0.f, 2.f,
        -1.f, 0.f, 1.f,

        -2.f, 0.f, 2.f,
        -4.f, 0.f, 4.f,
        -2.f, 0.f, 2.f,

        -1.f, 0.f, 1.f,
        -2.f, 0.f, 2.f,
        -1.f, 0.f, 1.f
    };
    tensor_t* kernel = new_tensor(kernel_shape, 3, kernel_values);
    return kernel;
}

tensor_t* sobel_y_3d()
{
    int kernel_shape[] = {3, 3, 3};
    float kernel_values[] = {
        -1.f, -2.f, -1.f,
        0.f,  0.f,  0.f,
        1.f,  2.f,  1.f,

        -2.f, -4.f, -2.f,
        0.f,  0.f,  0.f,
        2.f,  4.f,  2.f,

        -1.f, -2.f, -1.f,
        0.f,  0.f,  0.f,
        1.f,  2.f,  1.f
    };
    tensor_t* kernel = new_tensor(kernel_shape, 3, kernel_values);
    return kernel;
}

tensor_t* sobel_z_3d()
{
    int kernel_shape[] = {3, 3, 3};
    float kernel_values[] = {
        -1.f, -2.f, -1.f,
        -2.f, -4.f, -2.f,
        -1.f, -2.f, -1.f,

        0.f,  0.f,  0.f,
        0.f,  0.f,  0.f,
        0.f,  0.f,  0.f,

        1.f,  2.f,  1.f,
        2.f,  4.f,  2.f,
        1.f,  2.f,  1.f
    };
    tensor_t* kernel = new_tensor(kernel_shape, 3, kernel_values);
    return kernel;
}
