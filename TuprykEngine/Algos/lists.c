#include "lists.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


vector vector_create(size_t elem_size)
{
    vector v;
    v.elem_size = elem_size;
    v.size = 0;
    v.capacity = 4;
    v.data = malloc(v.capacity * elem_size);
    return v;
}

void vector_push(vector *v, void *element)
{
    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * v->elem_size);
    }

    void *dest = (char*)v->data + v->size * v->elem_size;
    memcpy(dest, element, v->elem_size);

    v->size++;
}

void* vector_get(vector *v, size_t index)
{
    return (char*)v->data + index * v->elem_size;
}

void vector_free(vector *v)
{
    free(v->data);
}

stack* stack_init(size_t elem_size)
{
    stack* s = (stack*) malloc(sizeof(stack));
    s->size = 0;
    s->next = NULL;
    s->elem_size = elem_size;
    return s;
}

void stack_push(stack* stack, void* value)
{
    // TODO:
}

void* stack_pop(stack* stack)
{
    // TODO:
    // struct stack_elem* tmp 
    // return
}

struct int_stack* int_stack_push(struct int_stack* stack, int value)
{
    struct int_stack* new_start = (struct int_stack*) malloc(sizeof(struct int_stack));
    new_start->value = value;
    new_start->next = stack;
    
    return new_start;
}

int int_stack_contains(struct int_stack* stack, int value)
{
    struct int_stack* tmp = stack;
    while (tmp != NULL)
    {
        if (tmp->value == value) return 1;
        tmp = tmp->next;
    }
    
    return 0;
}
