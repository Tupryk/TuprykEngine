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

struct int_queue* queue_push(struct int_queue* queue, int value)
{
    struct int_queue* new_start = (struct int_queue*) malloc(sizeof(struct int_queue));
    new_start->value = value;
    new_start->next = queue;
    
    return new_start;
}

int queue_contains(struct int_queue* queue, int value)
{
    struct int_queue* tmp = queue;
    while (tmp != NULL)
    {
        if (tmp->value == value) return 1;
        tmp = tmp->next;
    }
    
    return 0;
}
