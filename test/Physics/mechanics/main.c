#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/Graphics/image.h"
#include "../../../TuprykEngine/Graphics/raytracer.h"
#include "../../../TuprykEngine/Kinematics/configuration.h"
#include "../../../TuprykEngine/Physics/simulation.h"
#include "../../../TuprykEngine/Physics/forces.h"

#include "../../../TuprykEngine/scenes/devastator/desert.h"
#include "../../../TuprykEngine/visual/graphics/window.h"
#include "../../../TuprykEngine/visual/graphics/basic.h"
#include "../../../TuprykEngine/visual/prints/config.h"


int test_centroidal()
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
        
        sim_step(C, 0.01);
    }
    play_video(video_frames, frame_count);
    
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
    failures_count += test_centroidal();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    free_window();
    return 0;
}
