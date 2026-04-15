#define DEBUG

#include <stdlib.h>
#include <stdio.h>

#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/Geom/quaternions.h"


int test_rotmat()
{
    // TODO:
    printf("--------- Test Quaternion to Rotation Matrix ---------\n");
    int shape[] = {4, 1};
    float values[] = {1., 0., 0., 0.};
    tensor* q = new_tensor(shape, 2, values);

    tensor* mat = quaternion_to_matrix_give(q);

    printf("Quat:\n");
    print_tensor_verbose(q, 10);
    printf("---->\nMat:\n");
    print_tensor_verbose(mat, 10);

    tensor_free(q);
    tensor_free(mat);
    return 0;
}

int main()
{
    int failures_count = 0;
    failures_count += test_rotmat();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    return 0;
}
