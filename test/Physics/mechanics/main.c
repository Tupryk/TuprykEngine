#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/Graphics/image.h"
#include "../../../TuprykEngine/Graphics/raytracer.h"
#include "../../../TuprykEngine/Kinematics/configuration.h"
#include "../../../TuprykEngine/Kinematics/frame.h"
#include "../../../TuprykEngine/Physics/simulation.h"
#include "../../../TuprykEngine/Physics/forces.h"
#include "../../../TuprykEngine/Algos/lists.h"
#include "../../../TuprykEngine/Geom/quaternions.h"

#include "../../../TuprykEngine/scenes/devastator/desert.h"
#include "../../../TuprykEngine/visual/graphics/window.h"
#include "../../../TuprykEngine/visual/graphics/basic.h"
#include "../../../TuprykEngine/visual/prints/config.h"


int test_com()
{
    config* C = init_devastator_config();
    
    tensor* new_q = tensor_copy_shape(C->q);
    tensor_fill_uniform(new_q, -1.f, 1.f);
    new_q->values[0] = 0.f;
    new_q->values[1] = 0.f;
    new_q->values[2] = 1.f;
    new_q->values[3] = 1.f;
    new_q->values[4] = 0.f;
    new_q->values[5] = 0.f;
    new_q->values[6] = 0.f;
    config_set_q(C, new_q->values);
    tensor_fill_uniform(C->q_vel, -.1f, .1f);
    C->q_vel->values[0] = 0.f;
    C->q_vel->values[1] = 0.f;
    C->q_vel->values[2] = 0.f;
    C->q_vel->values[3] = -10.f;
    C->q_vel->values[4] = 10.f;
    C->q_vel->values[5] = 10.f;
    print_config(C);
    
    int frame_count = 10;
    tensor* com = new_tensor_vector(3, NULL);

    for (int i = 0; i < frame_count; i++)
    {
        printf("Rendering frame %d...\n", i+1);
        
        float total_mass = center_of_mass(C, 4, com);
        printf("COM: [%g, %g, %g], Total Mass: %g\n", com->values[0], com->values[1], com->values[2], total_mass);
        
        sim_step(C, 0.01);
    }
    
    config_free(C);
    tensor_free(new_q);
    tensor_free(com);
    return 0;
}

void add_trans_force(config* C, int frame_id, float* force_values, float* poa_values)
{
    force_t* f = (force_t*) malloc(sizeof(force_t));
    f->force = new_tensor_vector(3, force_values);
    f->torque = NULL;
    f->poa = new_tensor_vector(3, poa_values);
    f->frame_id = frame_id;
    stack_push(C->forces, f);
}

int test_forces()
{
    float tau = 0.02f;
    config* C = init_devastator_config();
    
    tensor* new_q = tensor_copy_shape(C->q);
    tensor_fill(new_q, 0.f);
    new_q->values[2] = 1.f;
    // quaternion_x(M_PI * 0.55, new_q->values + 3);
    config_set_q(C, new_q->values);
    tensor_fill(C->q_vel, 0.f);
    
    // float force_values[] = {0.f, 0.f, 1.f};
    // float poa_values[] = {0.f, 0.f, -0.1f};
    // add_trans_force(C, 11, force_values, poa_values);
    // add_trans_force(C, 23, force_values, poa_values);
    // float force_values_top[] = {0.f, 0.f, -20.f};
    // add_trans_force(C, 29, force_values_top, poa_values);
    // float force_values_down[] = {20.f, 0.f, 40.f};
    // float poa_values_body[] = {0.f, 0.f, -0.2f};
    // add_trans_force(C, 5, force_values_down, poa_values_body);
    // float force_values_side[] = {-20.f, 0.f, 0.f};
    // float poa_values_side[] = {-0.1f, 0.f, 0.f};
    // add_trans_force(C, 17, force_values_side, poa_values_side);

    force_t* f = (force_t*) malloc(sizeof(force_t));
    f->force = NULL;
    float torque_values[] = {10.f, 0.f, 0.f};
    f->torque = new_tensor_vector(3, torque_values);
    f->poa = NULL;
    f->frame_id = 5;
    stack_push(C->forces, f);

    print_config(C);
    
    int frame_count = 32;
    tensor* video_frames[frame_count];
    tensor* com = new_tensor_vector(3, NULL);

    for (int i = 0; i < frame_count; i++)
    {
        printf("Rendering frame %d...\n", i+1);
        
        float total_mass = center_of_mass(C, 4, com);
        printf("COM: [%g, %g, %g], Total Mass: %g\n", com->values[0], com->values[1], com->values[2], total_mass);
        
        tensor* im = new_image(WINDOW_W, WINDOW_H, 3);
        raytrace(C, -1, im);
        video_frames[i] = im;
        
        sim_step(C, tau);
        if (i == 0)
        {
            force_t* tor = stack_pop(C->forces);
            force_free(tor);
        }
    }
    play_video(video_frames, frame_count, tau);
    
    config_free(C);
    for (int i = 0; i < frame_count; i++)
    {
        tensor_free(video_frames[i]);
    }
    tensor_free(new_q);
    tensor_free(com);
    return 0;
}

int main(void)
{
    srand( time( NULL ) );
    init_window();

    int failures_count = 0;
    // failures_count += test_com();
    failures_count += test_forces();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    free_window();
    return 0;
}
