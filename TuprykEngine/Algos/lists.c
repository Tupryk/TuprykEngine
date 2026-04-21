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

stack* stack_init()
{
    stack* s = (stack*) malloc(sizeof(stack));
    s->size = 0;
    s->next = NULL;
    return s;
}

void stack_free(stack* s)
{
    while (s->next != NULL)
    {
        stack_pop(s);
    }
    free(s);
}

void stack_push(stack* s, void* data)
{
    struct stack_elem* new_elem = (struct stack_elem*) malloc(sizeof(struct stack_elem));
    new_elem->data = data;
    new_elem->next = s->next;
    s->next = new_elem;
    s->size++;
}

void* stack_pop(stack* s)
{
    if (s->next != NULL)
    {
        struct stack_elem* poped = s->next;
        void* data = poped->data;
        s->next = poped->next;
        free(poped);

        s->size--;
        return data;
    }
    return NULL;
}

int_stack* int_stack_init()
{
    int_stack* stack = (int_stack*) malloc(sizeof(int_stack));
    stack->size = 0;
    stack->next = NULL;
    return stack;
}

void int_stack_free(int_stack* s)
{
    while (s->next != NULL)
    {
        int_stack_pop(s);
    }
    free(s);
}

void int_stack_push(int_stack* s, int value)
{
    struct int_stack_elem* new_elem = (struct int_stack_elem*) malloc(sizeof(struct int_stack_elem));
    new_elem->value = value;
    new_elem->next = s->next;
    s->next = new_elem;
    s->size++;
}

int int_stack_pop(int_stack* s)
{
    if (s->next != NULL)
    {
        struct int_stack_elem* poped = s->next;
        int value = poped->value;
        s->next = poped->next;
        free(poped);

        s->size--;
        return value;
    }
    return -1;
}

int int_stack_contains(int_stack* s, int value)
{
    struct int_stack_elem* tmp = s->next;
    while (tmp != NULL)
    {
        if (tmp->value == value) return 1;
        tmp = tmp->next;
    }
    
    return 0;
}
