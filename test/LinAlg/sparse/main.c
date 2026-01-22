#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/visual/prints/linalg.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/LinAlg/sparse.h"


int test_basic()
{
    int n = 4;
    int shape[] = {n, n};
    struct tensor* t = new_tensor(shape, 2, NULL);
    tensor_identity(t);
    
    struct sparse* st = sparse_from_tensor(t);
    struct tensor* t0 = tensor_from_sparse(st);
    
    print_tensor(t);
    print_sparse(st);
    print_tensor(t0);

    int failure_count = 0;
    for (int i = 0; i < st->value_count; i++)
    {
        if (st->values[i] != 1.f || st->indices[i] != i * st->value_count + i)
        {
            failure_count++;
            break;
        }
    }

    tensor_free(t);
    sparse_free(st);
    tensor_free(t0);

    return failure_count;
}

int main()
{
    int failures_count = 0;

    failures_count += test_basic();

    if (failures_count > 0) {
        printf("\033[1;31mFailed %d test(s)!\033[0m\n", failures_count);
    } else {
        printf("\033[1;32mAll tests passed! :)\033[0m\n");
    }
    return 0;
}
