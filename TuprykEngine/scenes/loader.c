#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "loader.h"


struct children_list
{
    int count;
    int* ids;
};

struct children_list read_frame(config_t* C, FILE *file, frame_t* new_frame, int new_frame_id, int* current_id)
{
    struct children_list cl;
    cl.count = 0;
    cl.ids = NULL;

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
            return cl;
        }

        else if (strstr(line, "radius:"))
        {
            float radius;
            if (sscanf(line, " radius: %f;", &radius) == 1)
            {
                geom_t* ball_geom = (geom_t*) new_frame->data;
                *(float*)ball_geom->mesh = radius;
                ball_geom->mass = radius;
                
                tensor_free(ball_geom->inertia);
                float Ii = 2.f/5.f * ball_geom->mass * radius*radius;
                float inertia_values[] = {Ii, Ii, Ii};
                ball_geom->inertia = new_tensor_diagonal(3, inertia_values);
            }
        }

        else if (strchr(line, '(') && strchr(line, ')'))
        {
            // New frame!
            float pos[] = {0.f, 0.f, 0.f};
            float rot[] = {1.f, 0.f, 0.f, 0.f};

            char name[64];
            char type_str[16];

            if (sscanf(line, " %63[^ (](%15[^)])", name, type_str) != 2)
            {
                printf("Invalid frame declaration: %s\n", line);
                continue;
            }

            frame_t* child = frame_init(name, pos, rot);
            child->parent = new_frame_id;
            child->pos_rel = tensor_copy(child->pos);
            child->rot_rel = tensor_copy(child->rot);

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
                float radius = 1.f;
                
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
    
                ball_geom->tex->reflectance = 0.2f;
    
                child->data = (void*) ball_geom;
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
    
            if (cl.ids == NULL) cl.ids = (int*) malloc(sizeof(int));
            else cl.ids = realloc(cl.ids, sizeof(int) * cl.count+1);
            cl.ids[cl.count] = *current_id;
            cl.count++;

            struct children_list ccl = read_frame(C, file, child, (*current_id), current_id);
            child->children_count = ccl.count;
            child->children = ccl.ids;
        }
    }
    return cl;
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
    struct children_list cl = read_frame(C, file, root, 0, &frame_counter);
    root->children_count = cl.count;
    root->children = cl.ids;

    // Fill in joint array and qs
    int q_dim = 0;
    int q_vel_dim = 0;
    C->joints = NULL;
    for (int i = 0; i < C->frame_count; i++)
    {
        if (C->frames[i]->type == 4)
        {
            C->joints_count++;
            joint_t* joint_data = C->frames[i]->data;
            int joint_type = joint_data->type;
            if (joint_type == 0 || joint_type == 1 || joint_type == 2)
            {
                q_dim++;
                q_vel_dim++;
            }
            else if (joint_type == 3)
            {
                q_dim += 7;
                q_vel_dim += 6;
            }
            #ifdef DEBUG
            else
            {
                printf("Invalid joint type!\n");
                exit(EXIT_FAILURE);
            }
            #endif
        }
    }
    int index = 0;
    if (C->joints_count)
    {
        C->joints = (int*) malloc(sizeof(int)*C->joints_count);
        for (int i = 0; i < C->frame_count; i++)
        {
            if (C->frames[i]->type == 4)
            {
                C->joints[index] = i;
                index++;
            }
        }
    }
    
    C->q = new_tensor_vector(q_dim, NULL);
    C->q_min = new_tensor_vector(q_dim, NULL);
    C->q_max = new_tensor_vector(q_dim, NULL);
    C->q_vel = new_tensor_vector(q_vel_dim, NULL);
    tensor_fill(C->q_min, -1.f);
    tensor_fill(C->q_max, 1.f);

    fclose(file);
    return C;
}
