#include "raytracer.h"


void raytrace(struct config* C, struct frame* cam, struct tensor* out)
{
    // Store intersection depth for z-buffer
    // Loop through all frames in the config (Just spheres for now (also no KD/Oct trees yet!))
    struct tensor* depth;

    int height = out->shape[1];
    int width = out->shape[0];
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int indices[] = {x, y, 0};
            int idx = get_tensor_value_index(out, indices);
            
            out->values[idx  ] = 0.f;
            out->values[idx+1] = 0.f;
            out->values[idx+2] = 0.f;
        }
    }
}
