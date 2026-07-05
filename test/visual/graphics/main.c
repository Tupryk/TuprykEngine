#define DEBUG

#include <SDL.h>
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/basic.h"
// #include "../../../TuprykEngine/test/Optim/functions.h"


// void rotating_plot()
// {
//     plot_unit_func(quadratic_eval, 0.f, 0.f, 20.f);
//     draw_3d_point(0.5f, 0.5f, 0.5f);
//     draw_3d_unit_cube();

//     add_global_rot(0.0f, 0.01f, 0.0f);
// }

int main()
{
    init_window();
    
    // for (float i = 0.f; i < 1.f; i+=0.025f) {
    //         draw_line(0, 0, i, 1.f);
    //     }
        
    // for (float i = 0.f; i < 1.f; i+=0.025f) {
    //     draw_line(0, 0, 1.f, i);
    // }

    // window_wait();

    // draw_3d_unit_cube();

    // window_wait();

    // // Plot a function
    // quadratic_init();
    
    // plot_unit_func(quadratic_eval, 0.f, 0.f, 20.f);
    // window_wait();

    // window_wait_with_func(rotating_plot);
    
    // quadratic_free();

    // Draw oval
    tensor* pos = new_tensor_vector(2, NULL);
    float A_values[] = {
        1.f, .5f,
        0.f, 1.f
    };
    tensor* A = new_tensor_matrix(2, 2, A_values);

    draw_gaussian(pos, A, 100.f);
    
    window_wait();
    tensor_free(pos);
    tensor_free(A);

    free_window();
    return 0;
}
