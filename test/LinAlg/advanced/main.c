#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"


int test_lu_decomposition_single(struct tensor* A)
{
    printf("----- A -----\n");
    print_tensor(A);
    int operator_shape[] = {A->shape[0], A->shape[0]};
    struct tensor* P = new_tensor(operator_shape, 2, NULL);
    struct tensor* L = tensor_copy_shape(P);
    struct tensor* U = tensor_copy_shape(A);
    tensor_lu_decomp(A, P, L, U);
    struct tensor* PL = tensor_mult_give(P, L);
    struct tensor* A_reconstructed = tensor_mult_give(PL, U);

    printf("----- P -----\n");
    print_tensor(P);
    printf("----- L -----\n");
    print_tensor(L);
    printf("----- U -----\n");
    print_tensor(U);
    printf("----- A Reconstructed -----\n");
    print_tensor(A_reconstructed);

    int failure = !tensors_equal(A, A_reconstructed);

    free_tensor(P);
    free_tensor(L);
    free_tensor(U);
    free_tensor(PL);
    free_tensor(A_reconstructed);

    return failure;
}

int test_lu_decomposition()
{
    int failure_count = 0;

    printf("--- Test 1 a): LU Decomposition ---\n");
    
    int a_shape_0[] = {3, 3};  // CxR
    float a_values_0[] = {
        -2,  2, -1,
         6, -6,  7,
         3, -8,  4
    };
    struct tensor* A = new_tensor(a_shape_0, 2, a_values_0);

    failure_count += test_lu_decomposition_single(A);
    free_tensor(A);
    
    printf("--- Test 1 b): LU Decomposition ---\n");

    int a_shape_1[] = {4, 5};  // CxR
    float a_values_1[] = {
        0, 1, 2, 1, 2,
        1, 0, 0, 0, 1,
        2, 1, 2, 1, 5,
        1, 2, 4, 3, 6
    };
    A = new_tensor(a_shape_1, 2, a_values_1);

    failure_count += test_lu_decomposition_single(A);
    free_tensor(A);

    printf("--- Test 1 c): LU Decomposition ---\n");

    int a_shape_2[] = {3, 3};  // CxR
    float a_values_2[] = {
        -1.5,  2.1,  6.4,
           2,  3.9,  3.1,
           5,    3,    2,
    };
    A = new_tensor(a_shape_2, 2, a_values_2);

    failure_count += test_lu_decomposition_single(A);
    free_tensor(A);

    return failure_count;
}

int test_determinant()
{
    int failure_count = 0;

    printf("--- Test 2 a): Determinant ---\n");

    int a_shape_0[] = {2, 2};  // CxR
    float a_values_0[] = {
        8, 5,
        4, 5
    };
    struct tensor* A = new_tensor(a_shape_0, 2, a_values_0);

    float det = tensor_determinant(A);

    printf("----- A -----\n");
    print_tensor(A);
    printf("Determinant: %f\n", det);

    failure_count += det == 20.f ? 0 : 1;

    printf("--- Test 2 b): Determinant ---\n");

    int a_shape_1[] = {3, 3};  // CxR
    float a_values_1[] = {
         1, -2,  3,
         2,  0,  3,
         1,  5,  4
    };
    A = new_tensor(a_shape_1, 2, a_values_1);

    det = tensor_determinant(A);

    printf("----- A -----\n");
    print_tensor(A);
    printf("Determinant: %f\n", det);

    failure_count += det == 25.f ? 0 : 1;

    int a_shape_2[] = {4, 4};  // CxR
    float a_values_2[] = {
         1,  3,  1,  4,
         3,  9,  5, 15,
         0,  2,  1,  1,
         0,  4,  2,  3
    };
    A = new_tensor(a_shape_2, 2, a_values_2);

    det = tensor_determinant(A);

    printf("----- A -----\n");
    print_tensor(A);
    printf("Determinant: %f\n", det);

    failure_count += det == -4.f ? 0 : 1;

    return failure_count;
}

int main()
{
    int failures_count = 0;

    failures_count += test_lu_decomposition();
    failures_count += test_determinant();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
