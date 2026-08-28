#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"


void read_frame(config_t* C, FILE *file, frame_t* new_frame, int new_frame_id, int* current_id)
{
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        printf("%s\n", line);

        // Remove newline
        line[strcspn(line, "\n")] = 0;
        
        // Ignore empty lines
        if (line[0] == '\0')
            continue;

        // Start of an object
        if (strchr(line, '{'))
        {
            continue;
        }

        else if (strstr(line, "pos:"))
        {
            sscanf(
                line,
                "pos: [%f, %f, %f]",
                &new_frame->pos->values[0],
                &new_frame->pos->values[1],
                &new_frame->pos->values[2]
            );
        }

        else if (strstr(line, "rot:"))
        {
            sscanf(
                line,
                "rot: [%f, %f, %f, %f]",
                &new_frame->rot->values[0],
                &new_frame->rot->values[1],
                &new_frame->rot->values[2],
                &new_frame->rot->values[3]
            );
        }

        else if (strchr(line, '}'))
        {
            return;
        }

        else if (strstr(line, "type:"))
        {
            char type_str[16];

            if (sscanf(line, " type: %15s", type_str) == 1)
            {
                if (strcmp(type_str, "ball") == 0)
                {
                    printf("It's a ball!\n");
                }
                else if (strcmp(type_str, "free") == 0)
                {
                    printf("It's free!\n");
                }
            }
        }

        // else if (strstr(line, "radius"))
        // {
        //     float r;

        //     if (sscanf(line, " radius = %f;", &r) == 1)
        //     {
        //         printf("Radius = %f\n", r);
        //     }
        // }

        else if (strchr(line, '(') && strchr(line, ')'))
        {
            float pos[] = {0.f, 0.f, 0.f};
            float rot[] = {1.f, 0.f, 0.f, 0.f};

            char name[64];
            char type_str[16];

            if (sscanf(line, " %63[^ (](%15[^)])", name, type_str) != 2)
            {
                printf("Invalid frame declaration: %s\n", line);
                continue;
            }

            frame *child = frame_init(name, pos, rot);
            child->parent = new_frame_id;

            // 0: Marker, 1: Geometry, 2: Camera, 3: Light, 4: Joint
            if (strcmp(type_str, "mark") == 0)
            {
                child->type = 0;
            }
            else if (strcmp(type_str, "geom") == 0)
            {
                child->type = 1;

                geom_t* ball_geom = (geom_t*) malloc(sizeof(geom_t));
    
                ball_geom->type = 0;
                
                ball_geom->mesh = (void*) malloc(sizeof(float));
                *(float*)ball_geom->mesh = radius;
                ball_geom->mass = radius;
                
                float Ii = 2.f/5.f * ball_geom->mass * radius*radius;
                float inertia_values[] = {Ii, Ii, Ii};
                ball_geom->inertia = new_tensor_diagonal(3, inertia_values);

                ball_geom->tex = (texture*) malloc(sizeof(texture));

                ball_geom->tex->color[0] = 1.f;
                ball_geom->tex->color[1] = 0.f;
                ball_geom->tex->color[2] = 0.f;

                ball_geom->tex->ambient   = 0.5;
                ball_geom->tex->diffuse   = 1.f;
                ball_geom->tex->specular  = 0.5f;
                ball_geom->tex->shininess = 10.f;
                ball_geom->tex->reflectance = 0.f;
    
                geom->tex->reflectance = 0.2f;
    
                child->data = (void*) geom;
            }
            else if (strcmp(type_str, "cam") == 0)
            {
                child->type = 2;

                camera_t* cam_data = (camera_t*) malloc(sizeof(camera_t));
                
                cam_data->fx = 0.1f;
                cam_data->fy = 0.1f;
                
                child->data = (void*) cam_data;
            }
            else if (strcmp(type_str, "light") == 0)
            {
                child->type = 3;

                light_t* light_data = (light_t*) malloc(sizeof(light_t));
                
                light_data->intensity = 1.0f;
                
                child->data = (void*) light_data;
            }
            else if (strcmp(type_str, "joint") == 0)
            {
                child->type = 4;

                joint_t* joint_data = (joint_t*) malloc(sizeof(joint_t));

                joint_data->type = 3;
                joint_data->has_limits = 0;
                joint_data->q_id = 0;
                joint_data->q_delta_id = 0;
                joint_data->friction = 0.f;

                child->data = (void*) joint_data;
            }

            (*current_id)++;
            C->frames[*current_id] = child;
            read_frame(C, file, child, (*current_id), current_id);
        }
    }
}

config_t* load_scene(const char* path)
{
    FILE *file = fopen(path, "r");
    if (!file)
    {
        printf("File not found: %s\n", path);
        return NULL;
    }

    // Init config
    config_t* C = (config_t*) malloc(sizeof(config_t));
    C->forces = stack_init();
    C->gravity = new_tensor_vector(3, NULL);
    C->gravity->values[2] = -9.81f;

    // Count total frames
    char line[256];
    int frame_count = 0;
    while (fgets(line, sizeof(line), file))
    {
        if (strchr(line, '{')) frame_count++;
    }

    C->frame_count = frame_count + 1;
    C->frames = (frame_t**) malloc(sizeof(frame_t*) * C->frame_count);
    
    int frame_counter = 0;  // For recursive loop

    // Init origin frame
    float origin_pos[] = {0.f, 0.f, 0.f};
    float origin_rot[] = {1.f, 0.f, 0.f, 0.f};

    frame_t* root = frame_init("root", origin_pos, origin_rot);
    root->pos_rel = NULL;
    root->rot_rel = NULL;

    root->children_count = 0;
    root->children = NULL;
    
    root->data = NULL;

    root->parent = -1;
    root->type = 0;

    C->frames[0] = root;

    // Recursively go through frames
    rewind(file);
    read_frame(C, file, root, 0, &frame_counter);

    fclose(file);
    return C;
}
