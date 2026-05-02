#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../TuprykEngine/Graphics/image.h"
#include "../../TuprykEngine/Graphics/raytracer.h"
#include "../../TuprykEngine/Kinematics/configuration.h"
#include "../../TuprykEngine/Kinematics/frame.h"
#include "../../TuprykEngine/Planning/Motion/rrt.h"
#include "../../TuprykEngine/Algos/lists.h"

#include "../../TuprykEngine/visual/prints/config.h"
#include "../../TuprykEngine/visual/graphics/basic.h"
#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/scenes/devastator/desert.h"


int test_rrt()
{
    config* C = init_devastator_config();
    print_config(C);

    tensor* q0 = tensor_copy_shape(C->q);
    tensor* qT = tensor_copy_shape(C->q);
    tensor_fill_uniform(q0, -1.f, 1.f);
    tensor_fill_uniform(qT, -1.f, 1.f);

    printf("Planning a path...\n");
    stack* path = rrt_find_path(C, q0, qT, 0.1);
    int path_length = path->size;

    tensor* video_frames[path_length];
    for (int i = 0; i < path_length; i++)
    {
        printf("Rendering frame %d...\n", i+1);

        tensor* q = (tensor*) stack_pop(path);
        config_set_q(C, q);
        tensor_free(q);

        tensor* im = new_image(WINDOW_W, WINDOW_H, 3);
        raytrace(C, -1, im);
        video_frames[i] = im;
    }

    init_window();
    play_video(video_frames, path_length, 0.01);
    free_window();
    
    config_free(C);
    for (int i = 0; i < path_length; i++)
    {
        tensor_free(video_frames[i]);
    }
    return 0;
}

int main(void)
{
    srand( time( NULL ) );

    int failures_count = 0;
    failures_count += test_rrt();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    return 0;
}
