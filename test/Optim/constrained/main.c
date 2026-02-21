#define DEBUG
#define OPTIM_VERBOSE

#include <stdio.h>
#include "../functions.h"
#include "../../../TuprykEngine/Optim/meta.h"
#include "../../../TuprykEngine/Optim/constrained/nlp.h"
#include "../../../TuprykEngine/Optim/constrained/augmented_lagrangian.h"


int main()
{
    int shape[] = {2, 1};
    float values[] = {11.f, -5.f};
    struct tensor* init_x = new_tensor(shape, 2, values);
    
    quadratic_init();
    struct nlp* nlp_0 = get_nlp0();

    printf("--- Test 1: Augmented Lagrangian ---\n");
    aug_lagrangian_init(init_x, nlp_0, 1e-1, 1e-2, 100);
    aug_lagrangian_run();
    aug_lagrangian_free();
    
    nlp_free(nlp_0);
    tensor_free(init_x);
    quadratic_free();

    return 0;
}
