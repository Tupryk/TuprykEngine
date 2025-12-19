#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"


int main()
{
    printf("--- Test 1: Display Matrix ---\n");
    int shape_a[] = {3, 4};
    TYPE a_values[] = {-1.026171548179903, -0.458737870232199, 0.004127730314223807, 0.8360107337310204, 0.8292537172427407, 1.2425586400298345, 0.35083116511410856, -0.32207087467934237, 0.23016262279879746, 0.20249721733526918, 0.9171447250259418, 1.6059787055761126};
    struct tensor* a = new_tensor(shape_a, 2, a_values);
    printf("----- a -----\n");
    print_tensor_flat(a);
    print_tensor(a);
    free_tensor(a);

    printf("--- Test 2: Display Tensor ---\n");
    int shape_A[] = {2, 3, 4};
    TYPE A_values[] = {-0.4183943856061683, 0.649877273300988, -0.6483414912957177, 1.1464775201493558, -0.4143488692319995, -0.709383184839918, 1.2155562927818704, -1.0370901589362167, -1.2255448237849136, -0.18553331164986164, 0.9432446979868154, 1.3123782657029717, -1.365176458943696, 0.28054932429020507, -0.6690292509760933, 0.9156290572153675, -0.10335843043287168, -0.2727831596617291, -1.9935512577383825, -0.018331413015292647, 1.8971294863545771, -0.060868022316069, 0.6446161475254113, 1.0590849554953927};
    struct tensor* A = new_tensor(shape_A, 3, A_values);
    printf("----- A flat -----\n");
    print_tensor_flat(A);
    printf("----- A -----\n");
    print_tensor(A);
    printf("----- A verbose -----\n");
    print_tensor_verbose(A, 10);
    free_tensor(A);
    
    return 0;
}
