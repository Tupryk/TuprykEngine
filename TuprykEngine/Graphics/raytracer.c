#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raytracer.h"


void raytrace(config* C, int cam, tensor* out)
{
    int frame_count = C->frame_count;
    if (cam == -1)
    {
        for (int i = 0; i < C->frame_count; i++)
        {
            if (C->frames[i]->type == 2)
            {
                cam = i;
                break;
            }
        }
    }
    // Store intersection depth for z-buffer
    // Loop through all frames in the config (Just spheres for now (also no KD/Oct trees yet!))
    #ifdef DEBUG
    if (C->frames[cam]->type != 2)
    {
        printf("Input frame is not a camera!\n");
        exit(EXIT_FAILURE);
    }
    if (cam == -1)
    {
        printf("No cameras found!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    
    int width = out->shape[0];
    int height = out->shape[1];
    float depth[width][height];

    frame* cam_frame = C->frames[cam];
    frame* light_frame = C->frames[C->lights[0]];
    float f = *(float*) cam_frame->data;
    
    int cam_ray_shape[] = {3, 1};
    tensor* cam_ray = new_tensor(cam_ray_shape, 2, NULL);
    tensor* ball_to_cam = tensor_copy(cam_ray);
    tensor* hit1 = tensor_copy(cam_ray);
    tensor* hit2 = tensor_copy(cam_ray);

    tensor_sub(cam_frame->pos, light_frame->pos, hit1);
    float camera_light = vector_norm(hit1);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            int indices[] = {x, y, 0};
            int idx = get_tensor_value_index(out, indices);

            float r = 0.f;
            float g = 0.f;
            float b = 0.f;

            cam_ray->values[0] = (((float) x / (float) width) - 0.5f) * f;
            cam_ray->values[1] = 0.1f;
            cam_ray->values[2] = (((float) y / (float) height) - 0.5f) * f;

            for (int i = 0; i < frame_count; i++)
            {
                frame* ball_frame = C->frames[i];
                if (ball_frame->type == 1)
                {
                    float r2 = *(float*)ball_frame->data;
                    r2 *= r2;
                    tensor_sub(cam_frame->pos, ball_frame->pos, ball_to_cam);
                    float a = vector_squared_norm(cam_ray);
                    float b = -2.f * vector_dot(cam_ray, ball_to_cam);
                    float c = vector_squared_norm(ball_to_cam) - r2;
    
                    int hit = b*b - 4*a*c >= 0;
                    if (hit)
                    {
                        float t1 = (-b + sqrt( b*b - (4*a*c) )) / (2*a);
                        float t2 = (-b - sqrt( b*b - (4*a*c) )) / (2*a);

                        tensor_scalar_mult(cam_ray, t1, hit1);
                        tensor_scalar_mult(cam_ray, t2, hit2);
                        
                        float distance1 = vector_norm(hit1);
                        float distance2 = vector_norm(hit2);
                        
                        float light_ball;
                        float camera_ball;
                        if ( distance1 < distance2 )
                        {
                            camera_ball = distance1;
                            tensor_add(hit1, cam_frame->pos, hit1);
                            tensor_sub(hit1, light_frame->pos, hit1);
                            light_ball = vector_norm(hit1);
                        }
                        else
                        {
                            camera_ball = distance2;
                            tensor_add(hit2, cam_frame->pos, hit2);
                            tensor_sub(hit2, light_frame->pos, hit2);
                            light_ball = vector_norm(hit1);
                        }

                        float cos = ( pow(light_ball, 2) + pow(camera_ball, 2) - pow(camera_light, 2) ) / ( 2 * camera_ball * light_ball );

                        r = (1.f - cos) * 10.f;
                        if (r > 1.f) r = 1.f;
                        if (r < 0.f) r = 0.f;
                    }
                }
            }
            
            out->values[idx  ] = r;
            out->values[idx+1] = g;
            out->values[idx+2] = b;
        }
    }
}
