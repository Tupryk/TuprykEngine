#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "linalg.h"
#include "../../Algos/lists.h"


void print_spaces(int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("    ");
    }
}

void print_frame(config* C, frame* f, int id, int depth)
{
    if (depth > 8)
    {
        print_spaces(depth);
        printf("(...)\n");
        return;
    }
    print_spaces(depth);
    printf("\033[1;32m--- Frame %d: ", id);
    if (f->name != NULL) printf("%s", f->name);
    else printf("-no name-");
    printf(" (depth %d) ---\033[0m\n", depth);

    float* pos = f->pos->values;
    float* rot = f->rot->values;
    
    print_spaces(depth);
    printf("-> Position: [%g, %g, %g]\n", pos[0], pos[1], pos[2]);
    print_spaces(depth);
    printf("-> Rotation: [%g, %g, %g, %g]\n", rot[0], rot[1], rot[2], rot[3]);
    
    if (f->parent != -1)
    {
        float* pos_rel = f->pos_rel->values;
        float* rot_rel = f->rot_rel->values;
        
        print_spaces(depth);
        printf("-> Rel. Position: [%g, %g, %g]\n", pos_rel[0], pos_rel[1], pos_rel[2]);
        print_spaces(depth);
        printf("-> Rel. Rotation: [%g, %g, %g, %g]\n", rot_rel[0], rot_rel[1], rot_rel[2], rot_rel[3]);
    }

    print_spaces(depth);
    printf("-> Type: ");
    switch(f->type)
    {
        case 0:
            printf("Marker\n");
            break;
        case 1:
            printf("Ball\n");
            geom* ball_geom = f->data;
            float r = *(float*) ball_geom->mesh;
            print_spaces(depth);
            printf("    -> Radius: %g\n", r);
            print_spaces(depth);
            printf("    -> Mass: %g\n", ball_geom->mass);
            print_spaces(depth);
            printf("    -> Inertia Tensor:\n");
            float* I = ball_geom->inertia->values;
            print_spaces(depth);
            printf("        [%g, %g, %g]\n", I[0], I[1], I[2]);
            print_spaces(depth);
            printf("        [%g, %g, %g]\n", I[3], I[4], I[5]);
            print_spaces(depth);
            printf("        [%g, %g, %g]\n", I[6], I[7], I[8]);
            break;
        case 2:
            printf("Camera\n");
            camera_t* cam_data = f->data;
            print_spaces(depth);
            printf("    -> Fx, Fy: %g, %g\n", cam_data->fx, cam_data->fy);
            break;
        case 3:
            printf("Light\n");
            light_t* light_data = f->data;
            print_spaces(depth);
            printf("    -> Intensity: %g\n", light_data->intensity);
            break;
        case 4:
            printf("Joint\n");
            joint_t* joint_data = f->data;
            int joint_type = joint_data->type;
            int q_id = joint_data->q_id;
            print_spaces(depth);
            printf("    -> Type: ");
            switch (joint_type)
            {
            case 0:
                printf("Hinge-X");
                break;
            case 1:
                printf("Hinge-Y");
                break;
            case 2:
                printf("Hinge-Z");
                break;
            case 3:
                printf("Free");
                break;
            default:
                printf("Unknown");
                break;
            }
            printf("\n");
            print_spaces(depth);
            printf("    -> q_id: %d\n", q_id);
            print_spaces(depth);
            printf("    -> q_value(s): [");
            if (joint_type == 0 || joint_type == 1 || joint_type == 2)
            {
                printf("%g", C->q->values[q_id]);
            }
            else if (joint_type == 3)
            {
                float* q = C->q->values;
                printf(
                    "%g, %g, %g, %g, %g, %g, %g",
                    q[q_id], q[q_id+1], q[q_id+2], q[q_id+3], q[q_id+4], q[q_id+5], q[q_id+6]
                );
            }
            printf("]\n");
            break;
        default:
            printf("Unknown (%d)\n", f->type);
            break;
    }

    print_spaces(depth);
    printf("-> Children: [");
    for (int i = 0; i < f->children_count; i++)
    {
        printf("%d, ", f->children[i]);
    }
    printf("]\n\n");

    for (int i = 0; i < f->children_count; i++)
    {
        int child_id = f->children[i];
        print_frame(C, C->frames[child_id], child_id, depth+1);
    }
}

void print_config(config* C)
{
    printf("Total frame count: %d\n", C->frame_count);
    printf("Joints in scene (%d): [", C->joints_count);
    for (int i = 0; i < C->joints_count; i++)
    {
        printf("%d, ", C->joints[i]);
    }
    printf("]\n");
    printf("Lights in scene (%d): [", C->lights_count);
    for (int i = 0; i < C->lights_count; i++)
    {
        printf("%d, ", C->lights[i]);
    }
    printf("]\n");
    if (C->frame_count)
    {
        print_frame(C, C->frames[0], 0, 0);
    }
    if (config_colliding(C))
    {
        printf("There are collisions in the scene!\n");
    }
    else
    {
        printf("There are no collisions in the scene.\n");
    }
    if (C->q != NULL)
    {
        int q_dim = C->q->volume;
        printf("q_dim: %d\n", q_dim);
        printf("q: [");
        for (int i = 0; i < q_dim; i++)
        {
            printf("%g, ", C->q->values[i]);
        }
        printf("]\n");
        printf("q_min: [");
        for (int i = 0; i < q_dim; i++)
        {
            printf("%g, ", C->q_min->values[i]);
        }
        printf("]\n");
        printf("q_max: [");
        for (int i = 0; i < q_dim; i++)
        {
            printf("%g, ", C->q_max->values[i]);
        }
        printf("]\n");
        printf("q_vel: [");
        for (int i = 0; i < C->q_vel->volume; i++)
        {
            printf("%g, ", C->q_vel->values[i]);
        }
        printf("]\n");
    }
    printf("Forces on Configuration: %zu\n", C->forces->size);
    struct stack_elem* current_elem = C->forces->next;
    while (current_elem != NULL)
    {
        force_t* force = (force_t*) current_elem->data;
        printf("+------- Force on Frame %d -------+\n", force->frame_id);
        if (force->force != NULL) printf("Force: [%g, %g, %g]\n", force->force->values[0], force->force->values[1], force->force->values[2]);
        if (force->torque != NULL) printf("Torque: [%g, %g, %g]\n", force->torque->values[0], force->torque->values[1], force->torque->values[2]);
        if (force->poa != NULL) printf("POA: [%g, %g, %g]\n", force->poa->values[0], force->poa->values[1], force->poa->values[2]);

        current_elem = current_elem->next;
    }
    printf("\n");
}
