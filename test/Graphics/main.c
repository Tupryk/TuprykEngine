#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../TuprykEngine/Graphics/image.h"
#include "../../TuprykEngine/Graphics/raytracer.h"
#include "../../TuprykEngine/Kinematics/configuration.h"

#include "../../TuprykEngine/scenes/devastator/desert.h"
#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/visual/graphics/basic.h"
#include "../../TuprykEngine/visual/prints/config.h"


int test_raytracer()
{
    tensor* im = new_image(WINDOW_W, WINDOW_H, 3);
    tensor_fill_uniform(im, 0.f, 1.f);

    config* C = init_devastator_config();
    print_config(C);
    
    raytrace(C, -1, im);
    
    view_image(im);
    window_wait();
    
    config_free(C);
    tensor_free(im);
    return 0;
}

int main(void)
{
    init_window();

    int failures_count = 0;
    failures_count += test_raytracer();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    free_window();
    return 0;
}
