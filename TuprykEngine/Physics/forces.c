#include <stdio.h>
#include <stdlib.h>

#include "forces.h"
#include "../Algos/lists.h"
#include "../Geom/quaternions.h"


tensor* compute_mass_matrix(config* C)
{
    // Maps joint accelerations to generalised forces
    // MuJoCo computes M efficiently via the Composite Rigid Body (CRB) algorithm
    tensor* M = new_tensor_matrix(C->q->shape[0], C->q_vel->shape[0], NULL);

    // TODO:
    
    return M;
}

float center_of_mass(config* C, int root, tensor* com)
{
    #ifdef DEBUG
    if (com->shape_dim != 2 || com->shape[0] != 3 || com->shape[1] != 1)
    {
        printf("Invalid COM vector for output!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    
    float total_mass = 0.f;

    int bodies_count = 0;
    int body_ids[C->frame_count];

    int_stack* frames_to_check = int_stack_init();
    int_stack_push(frames_to_check, root);

    while (frames_to_check->next != NULL)
    {
        int current_frame_id = int_stack_pop(frames_to_check);
        frame* current_frame = C->frames[current_frame_id];

        int children_count = current_frame->children_count;
        for (int i = 0; i < children_count; i++)
        {
            int_stack_push(frames_to_check, current_frame->children[i]);
        }
        if (current_frame->type == 1)
        {
            geom* frame_data = (geom*) current_frame->data;
            total_mass += frame_data->mass;

            body_ids[bodies_count] = current_frame_id;
            bodies_count++;
        }
    }
    int_stack_free(frames_to_check);

    tensor* tmp = new_tensor_vector(3, NULL);
    tensor_fill(com, 0.f);

    for (int i = 0; i < bodies_count; i++)
    {
        frame* current_frame = C->frames[body_ids[i]];
        geom* frame_data = (geom*) current_frame->data;

        float perc = frame_data->mass / total_mass;
        tensor_scalar_mult(current_frame->pos, perc, tmp);
        tensor_add(com, tmp, com);
    }

    tensor_free(tmp);

    return total_mass;
}

void combined_inertia(config* C, int root, tensor* com, tensor* I_cm)
{
    #ifdef DEBUG
    if (com->shape_dim != 2 || com->shape[0] != 3 || com->shape[1] != 1)
    {
        printf("Invalid COM vector as input!\n");
        exit(EXIT_FAILURE);
    }
    if (com->shape_dim != 2 || com->shape[0] != 3 || com->shape[1] != 1)
    {
        printf("Invalid I_cm matrix for output!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    // TODO: This first part should maybe be a sepparate function
    // as it also gets used by the COM computation.
    int bodies_count = 0;
    int body_ids[C->frame_count];

    int_stack* frames_to_check = int_stack_init();
    int_stack_push(frames_to_check, root);

    while (frames_to_check->next != NULL)
    {
        int current_frame_id = int_stack_pop(frames_to_check);
        frame* current_frame = C->frames[current_frame_id];

        int children_count = current_frame->children_count;
        for (int i = 0; i < children_count; i++)
        {
            int_stack_push(frames_to_check, current_frame->children[i]);
        }
        if (current_frame->type == 1)
        {
            body_ids[bodies_count] = current_frame_id;
            bodies_count++;
        }
    }
    int_stack_free(frames_to_check);

    tensor_fill(I_cm, 0.f);
    tensor* I_i_cm = new_tensor_matrix(3, 3, NULL);
    tensor* D = new_tensor_matrix(3, 3, NULL);
    tensor* d = new_tensor_vector(3, NULL);

    for (int i = 0; i < bodies_count; i++)
    {
        frame* current_frame = C->frames[body_ids[i]];
        geom* frame_data = (geom*) current_frame->data;
        
        tensor_sub(current_frame->pos, com, d);
        float dx = d->values[0];
        float dy = d->values[1];
        float dz = d->values[2];
        float dx2 = dx*dx;
        float dy2 = dy*dy;
        float dz2 = dz*dz;

        // Parallel axis theorem
        float D_values[] = {
            dy2 + dz2, -dx *  dy, -dx *  dz,
            -dx *  dy, dx2 + dz2, -dy *  dz,
            -dx *  dz, -dy *  dz, dx2 + dy2
        };
        tensor_set_values(D, D_values);
        tensor_scalar_mult(D, frame_data->mass, D);

        tensor_add(frame_data->inertia, D, I_i_cm);
        tensor_add(I_cm, I_i_cm, I_cm);
    }

    tensor_free(I_i_cm);
    tensor_free(D);
    tensor_free(d);
}

// void centroidal_forces(config* C, int root, tensor* force, tensor* torque)
// {
//     // TODO:
// }
