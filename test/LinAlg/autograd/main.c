#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/ui/prints/linalg.h"

int test_linear()
{
    int failure = 0;
    printf("--- Test 1: Linear Function ---\n");

    float x_values[] = { 3.f };
    tensor_t* x = new_tensor_vector(1, values);
    tensor_use_grad(x);

    // y = x**2 + 5
    tensor_scalar_pow(x, 2.f, y);
    tensor_scalar_add(y, 5.f, y);

    tensor_t* grad = tensor_backward(y);
    if (fabsf(grad->values[0] - 0.6f) > 0.001) failure++;

    if (failure > 0) {
        printf("\033[1;31mFail\033[0m\n");
    } else {
        printf("\033[1;32mSuccess\033[0m\n");
    }

    tensor_free(x);
    tensor_free(y);
    tensor_free(grad);

    return failure;
}

int main()
{
    int failures_count = 0;

    failures_count += test_linear();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
