#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include "../../TuprykEngine/Algos/lists.h"
#include "../../TuprykEngine/visual/prints/algos.h"


int test_vector()
{
    vector v = vector_create(sizeof(int));

    for (int i = 0; i < 10; i++)
    {
        vector_push(&v, &i);
    }

    print_int_vector(&v);

    vector_free(&v);

    return 0;
}

int test_int_stack()
{
    int_stack* s = int_stack_init();

    for (int i = 0; i < 10; i++)
    {
        int_stack_push(s, i);
    }

    print_int_stack(s);
    printf("Contains 5?: %d\n", int_stack_contains(s, 5));
    printf("Contains 11?: %d\n", int_stack_contains(s, 11));

    for (int i = 0; i < 10; i++)
    {
        printf("%d\n", int_stack_pop(s));
    }

    print_int_stack(s);

    int_stack_free(s);
 
    return 0;
}

int test_stack()
{
    stack* s = stack_init();

    int* v1 = malloc(sizeof(int));
    v1[0] = 1;

    int* v2 = malloc(sizeof(int));
    v2[0] = 2;

    int* v3 = malloc(sizeof(int));
    v3[0] = 3;

    stack_push(s, v1);
    stack_push(s, v2);
    stack_push(s, v3);

    for (int i = 0; i < 3; i++)
    {
        int v = *(int*) stack_pop_at_index(s, s->size-1);
        printf("%d\n", v);
    }

    free(v1);
    free(v2);
    free(v3);
    stack_free(s);
    return 0;
}

int main()
{
    // test_vector();
    // test_int_stack();
    test_stack();
    return 0;
}
