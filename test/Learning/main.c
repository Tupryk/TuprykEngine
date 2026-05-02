#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../TuprykEngine/Learning/mlp.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/visual/prints/linalg.h"


int test_mlp()
{
    int input_dim = 3;
    int output_dim = 2;

    int layer_count = 4;
    int layer_sizes[] = {input_dim, 4, 4, output_dim};
    int activation_types[] = {0, 1, 1, 0};
    mlp_t* mlp = mlp_init(layer_count, layer_sizes, activation_types);

    tensor* input = new_tensor_vector(input_dim, NULL);
    tensor* output = new_tensor_vector(output_dim, NULL);
    tensor_fill_uniform(input, -1.f, 1.f);
    
    mlp_forward(mlp, input, output);
    printf("Given input: \n");
    print_tensor(input);
    printf("Model outputs: \n");
    print_tensor(output);
    
    tensor_free(input);
    tensor_free(output);

    mlp_free(mlp);
    return 0;
}

int main(void)
{
    srand( time( NULL ) );

    int failures_count = 0;
    failures_count += test_mlp();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }

    return 0;
}
