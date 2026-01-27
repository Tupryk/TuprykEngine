#define DEBUG
#define OPTIM_VERBOSE

#include "../../TuprykEngine/Optim/meta.h"
#include "../../TuprykEngine/Optim/unconstrained/gradient_descent.h"
#include "../../TuprykEngine/Optim/unconstrained/newton.h"
#include "../../TuprykEngine/Optim/functions.h"
#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/visual/graphics/basic.h"


// init_window();
// draw_3d_unit_cube();
// plot_unit_func(quadratic_eval, 0.f, 0.f, 20.f);
// SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
// for (int i = 0; i < logs->steps-1; i++)
// {
//     struct tensor* x_0 = tensor_slice(logs->Xs, i);
//     struct tensor* x_1 = tensor_slice(logs->Xs, i+1);
//     draw_line_3d(
//         x_0->values[0], x_0->values[1], logs->costs->values[i],
//         x_1->values[0], x_1->values[1], logs->costs->values[i+1]
//     );
//     tensor_free(x_0);
//     tensor_free(x_1);
// }
// window_wait();
// free_window();

int main()
{
    int shape[] = {2, 1};
    float values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 2, values);
    
    quadratic_init();

    printf("--- Test 1: Gradient Descent ---\n");
    struct optim_logs* logs_gd = gradient_descent(init_x, quadratic_eval, quadratic_eval2, 1e-1, 1e-2, 100);

    printf("--- Test 2: Newton ---\n");
    struct optim_logs* logs_newton = newton(init_x, quadratic_eval, quadratic_eval2, quadratic_eval3, 1, 1e-2, 100);

    printf("--- Test 3: Gauss Newton ---\n");
    struct optim_logs* logs_gnew = gauss_newton(init_x, quadratic_eval, quadratic_eval2, 1, 1e-2, 100);

    tensor_free(init_x);
    optim_logs_free(logs_gd);
    optim_logs_free(logs_newton);
    quadratic_free();

    return 0;
}
