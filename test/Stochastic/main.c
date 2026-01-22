#define DEBUG

#include <stdio.h>
#include "../../TuprykEngine/Stochastic/markov.h"
#include "../../TuprykEngine/LinAlg/sparse.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/visual/prints/linalg.h"


int test_basic()
{
    int shape[] = {3, 3};
    float values[] = {
        1.f/4.f, 1.f/4.f, 2.f/4.f,
        1.f/3.f, 2.f/3.f, 0.f,
        0.f, 1.f, 0.f
    };
    struct tensor* t = new_tensor(shape, 2, values);
    struct sparse* st = sparse_from_tensor(t);
    tensor_free(t);
    print_sparse(st);

    struct markov_chain* mc = new_markov_chain(st, 0);

    markov_chain_steps(mc, 1000);

    for (int i = 0; i < mc->state_count; i++)
    {
        printf("%d, ", mc->visited_counts[i]);
    }
    printf("\nCurrent state: %d\n", mc->current_state);

    markov_chain_free(mc);
    return 0;
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
