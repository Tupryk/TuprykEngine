#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/ui/prints/linalg.h"

int test_linear()
{
    printf("--- Test 1: Linear Function ---\n");

    int failure = 0;

    if (failure > 0) {
        printf("\033[1;31mFail\033[0m\n");
    } else {
        printf("\033[1;32mSuccess\033[0m\n");
    }

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
