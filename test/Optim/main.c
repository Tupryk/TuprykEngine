#define DEBUG
#define OPTIM_VERBOSE

#include <stdlib.h>
#include <stdio.h>
#include "../../TuprykEngine/visual/prints/linalg.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/Optim/unconstrained/newton.h"
#include "../../TuprykEngine/Optim/unconstrained/gradient_descent.h"
#include "../../TuprykEngine/Optim/functions.h"


int main()
{
    int shape[] = {2, 1};
    float values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 2, values);
    
    quadratic_init();

    printf("--- Test 1: Gradient Descent ---\n");
    gradient_descent(init_x, quadratic_eval, quadratic_eval2, 1e-1, 1e-2, 100);
    
    printf("--- Test 2: Newton ---\n");
    newton(init_x, quadratic_eval, quadratic_eval2, quadratic_eval3, 1, 1e-2, 100);

    tensor_free(init_x);

    quadratic_free();

    return 0;
}
