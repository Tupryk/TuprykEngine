#define DEBUG

#include <stdio.h>
#include <stdlib.h>
#include "../../TuprykEngine/Algos/lists.h"
#include "../../TuprykEngine/visual/prints/algos.h"

int main()
{
    vector v = vector_create(sizeof(int));

    for (int i = 0; i < 10; i++)
    {
        vector_push(&v, &i);
    }

    print_int_vector(&v);
}
