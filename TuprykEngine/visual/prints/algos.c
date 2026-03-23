#include "algos.h"
#include <stdio.h>


void print_int_vector(vector* v)
{
    printf("(%d) [", (int)v->size);
    for (size_t i = 0; i < v->size; i++)
    {
        int value = *(int*)vector_get(v, i);
        if (i == v->size-1) printf("%d", value);
        else printf("%d, ", value);
    }
    printf("]\n");
}
