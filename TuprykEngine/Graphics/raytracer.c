#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raytracer.h"
#include "../Algos/utils.h"
#include "../visual/prints/linalg.h"

// #define SHADOWS


float ray_ball_hit(tensor* cam_pos, tensor* ray_dir, tensor* ball_pos, float radius, tensor* out)
{
    tensor* ball_to_cam = tensor_sub_give(cam_pos, ball_pos);

    float a = vector_squared_norm(ray_dir);
    float b = 2.f * vector_dot(ray_dir, ball_to_cam);
    float c = vector_squared_norm(ball_to_cam) - radius * radius;

    tensor_free(ball_to_cam);

    float disc = b*b - 4*a*c;
    if (disc < 0) return -1.f;

    float sqrt_disc = sqrt(disc);

    float t1 = (-b + sqrt_disc) / (2.f * a);
    float t2 = (-b - sqrt_disc) / (2.f * a);

    if (t1 < 0 && t2 < 0) return -1.f;
    float camera_ball_dist = (t2 > 0 && t2 < t1) ? t2 : (t1 > 0 ? t1 : t2);
    // TODO: camera inside ball case

    if (out != NULL)
    {
        tensor_scalar_mult(ray_dir, camera_ball_dist, out);
        tensor_add(out, cam_pos, out);
    }

    return camera_ball_dist;
}

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

    // TODO: free tensors
    
    int width = out->shape[0];
    int height = out->shape[1];
    float depth[width][height];

    frame* cam_frame = C->frames[cam];
    frame* light_frame = C->frames[C->lights[0]];
    light_t* light_data = (light_t*) light_frame->data;
    camera_t* cam_data = (camera_t*) cam_frame->data;
    float fx = cam_data->fx;
    float fy = cam_data->fy;
    
    int cam_ray_shape[] = {3, 1};
    tensor* cam_ray = new_tensor(cam_ray_shape, 2, NULL);
    tensor* ball_hit = tensor_copy_shape(cam_ray);

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            depth[x][y] = 1000.0;

            int indices[] = {x, y, 0};
            int idx = get_tensor_value_index(out, indices);

            float R = 0.f;
            float G = 0.f;
            float B = 0.f;

            // TODO: Compute light rays according to camera orientation + use fx and fy properly
            cam_ray->values[0] = (((float) x / (float) width) - 0.5f) * fx;
            cam_ray->values[1] = 0.1f;
            cam_ray->values[2] = (((float) y / (float) height) - 0.5f) * fy * -1.f;
            vector_normalize(cam_ray);

            for (int i = 0; i < frame_count; i++)
            {
                frame* ball_frame = C->frames[i];
                if (ball_frame->type == 1)
                {
                    geom* ball_geom = (geom*) ball_frame->data;
                    texture* ball_tex = ball_geom->tex;

                    float  radius    = *(float*) ball_geom->mesh;
                    float* color     = ball_tex->color;
                    float  ambient   = ball_tex->ambient;
                    float  diffuse   = ball_tex->diffuse;
                    float  specular  = ball_tex->specular;
                    float  shininess = ball_tex->shininess;

                    float camera_ball_dist = ray_ball_hit(cam_frame->pos, cam_ray, ball_frame->pos, radius, ball_hit);

                    if (camera_ball_dist != -1.f && depth[x][y] > camera_ball_dist)
                    {
                        depth[x][y] = camera_ball_dist;
                        
                        tensor* ball_normal = tensor_sub_give(ball_hit, ball_frame->pos);
                        tensor* light_dir = tensor_sub_give(light_frame->pos, ball_hit);
                        tensor* cam_dir = tensor_sub_give(cam_frame->pos, ball_hit);
                        
                        vector_normalize(ball_normal);
                        vector_normalize(light_dir);
                        vector_normalize(cam_dir);
                        
                        tensor* light_reflect = tensor_copy(ball_normal);
                        
                        tensor_scalar_mult(light_reflect, vector_dot(light_dir, ball_normal), light_reflect);
                        tensor_scalar_mult(light_reflect, 2.f, light_reflect);
                        tensor_sub(light_reflect, light_dir, light_reflect);
                        
                        vector_normalize(light_reflect);

                        float diff = fmax(vector_dot(ball_normal, light_dir), 0.f);
                        float spec = powf(fmax(vector_dot(light_reflect, cam_dir), 0.f), shininess);

                        // TODO: make nicer vvvvvv
                        float intensity = diff * diffuse * light_data->intensity + ambient;
                        intensity = clip(intensity, 0.f, 1.f);
                        
                        R = color[0] * intensity + spec * specular;
                        G = color[1] * intensity + spec * specular;
                        B = color[2] * intensity + spec * specular;
                        // TODO: make nicer ^^^^^^

                        R = clip(R, 0.f, 1.f);
                        G = clip(G, 0.f, 1.f);
                        B = clip(B, 0.f, 1.f);

                        // Check for shadows
                        #ifdef SHADOWS
                        tensor* offset = tensor_scalar_mult_give(light_dir, 0.001);
                        tensor_add(ball_hit, offset, ball_hit);
                        tensor_free(offset);
                        float shadow_scaler = 0.5f;
                        for (int j = 0; j < frame_count; j++)
                        {
                            frame* ball_frame_ = C->frames[j];
                            if (ball_frame_->type == 1)
                            {
                                geom* ball_geom_ = (geom*) ball_frame_->data;
                                float radius_  = *(float*)ball_geom_->mesh;
                                float shadower = ray_ball_hit(ball_hit, light_dir, ball_frame_->pos, radius_, NULL);
                                if (shadower != -1)
                                {
                                    R *= shadow_scaler; G *= shadow_scaler; B *= shadow_scaler;
                                    break;
                                }
                            }
                        }
                        #endif

                        tensor_free(ball_normal);
                        tensor_free(light_dir);
                        tensor_free(cam_dir);
                        tensor_free(light_reflect);
                    }
                }
            }
            
            out->values[idx  ] = R;
            out->values[idx+1] = G;
            out->values[idx+2] = B;
        }
    }
}
