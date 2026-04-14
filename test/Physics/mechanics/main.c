#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"


int test_centroidal()
{
    return 0;
}

int main()
{
    int failures_count = 0;
    failures_count += test_centroidal();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    return 0;
}
