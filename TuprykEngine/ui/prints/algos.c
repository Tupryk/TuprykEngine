#include "algos.h"
#include <stdio.h>


void print_int_vector(vector_t* v)
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

void print_int_stack(int_stack_t* stack)
{
    struct int_stack_elem* elem = stack->next;
    printf("Stack (%zu): [", stack->size);
    while (elem != NULL)
    {
        printf("%d, ", elem->value);
        elem = elem->next;
    }
    printf("]\n");
}

void print_stack(pstack_t* stack, void (*elem_print)(void* elem))
{
    struct stack_elem* elem = stack->first;
    printf("Stack (%zu): [", stack->size);
    while (elem != NULL)
    {
        elem_print(elem->data);
        if (elem != stack->last) printf(", ");
        elem = elem->next;
    }
    printf("]\n");
}
