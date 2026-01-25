#define DEBUG
#define OPTIM_VERBOSE

#include "../../TuprykEngine/Optim/meta.h"
#include "../../TuprykEngine/Optim/unconstrained/gradient_descent.h"
#include "../../TuprykEngine/Optim/unconstrained/newton.h"
#include "../../TuprykEngine/Optim/functions.h"
#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/visual/graphics/basic.h"


int main()
{
    int shape[] = {2, 1};
    float values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 2, values);
    
    init_window();
    quadratic_init();

    printf("--- Test 1: Gradient Descent ---\n");
    struct optim_logs* logs_gd = gradient_descent(init_x, quadratic_eval, quadratic_eval2, 1e-1, 1e-2, 100);

    printf("--- Test 2: Newton ---\n");
    struct optim_logs* logs_newton = newton(init_x, quadratic_eval, quadratic_eval2, quadratic_eval3, 1, 1e-2, 100);

    tensor_free(init_x);
    optim_logs_free(logs_gd);
    optim_logs_free(logs_newton);
    quadratic_free();
    free_window();

    return 0;
}
