#include "raytracer.h"


void raytrace(config* C, frame* cam, tensor* out)
{
    // Store intersection depth for z-buffer
    // Loop through all frames in the config (Just spheres for now (also no KD/Oct trees yet!))
    
    int width = out->shape[0];
    int height = out->shape[1];
    float depth[width][height];
    int frame_count = C->frame_count;
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int indices[] = {x, y, 0};
            int idx = get_tensor_value_index(out, indices);

            float r = 0.f;
            float g = 0.f;
            float b = 0.f;

            for (int i = 0; i < frame_count; i++)
            {

            }
            
            out->values[idx  ] = r;
            out->values[idx+1] = g;
            out->values[idx+2] = b;
        }
    }
}
