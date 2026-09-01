#include "../global.h"
#include "lists.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


vector_t vector_init(size_t elem_size)
{
    vector_t v;
    v.elem_size = elem_size;
    v.size = 0;
    v.capacity = 4;
    v.data = malloc(v.capacity * elem_size);
    return v;
}

void vector_push(vector_t* v, void* element)
{
    if (v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = realloc(v->data, v->capacity * v->elem_size);
    }

    void *dest = (char*) v->data + v->size * v->elem_size;
    memcpy(dest, element, v->elem_size);

    v->size++;
}

void vector_set(vector_t* v, size_t index, void* element)
{
    void* dest = (char*)v->data + index * v->elem_size;
    memcpy(dest, element, v->elem_size);
}

void* vector_get(vector_t* v, size_t index)
{
    return (char*)v->data + index * v->elem_size;
}

void vector_free(vector_t* v)
{
    free(v->data);
}

pstack_t* stack_init()
{
    pstack_t* s = (pstack_t*) malloc(sizeof(stack_t));
    s->size = 0;
    s->next = NULL;
    return s;
}

void stack_free(pstack_t* s, void (*elem_freer)(void*))
{
    while (s->next != NULL)
    {
        void* elem = stack_pop(s);
        if (elem_freer != NULL) elem_freer(elem);
    }
    free(s);
}

void stack_push(pstack_t* s, void* data)
{
    struct stack_elem* new_elem = (struct stack_elem*) malloc(sizeof(struct stack_elem));
    new_elem->data = data;
    new_elem->next = s->next;
    s->next = new_elem;
    s->size++;
}

void* stack_pop(pstack_t* s)
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

void* stack_pop_at_index(pstack_t* s, size_t i)
{
    #ifdef DEBUG
    if (i >= s->size)
    {
        printf("Stack index out of range!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    
    size_t index = 0;
    struct stack_elem* prev_elem = NULL;
    struct stack_elem* current_elem = s->next;
    while (current_elem != NULL)
    {
        if (index == i)
        {
            void* data = current_elem->data;
            
            if (prev_elem == NULL) s->next = current_elem->next;
            else prev_elem->next = current_elem->next;
            
            free(current_elem);
            s->size--;
            
            return data;
        }
        prev_elem = current_elem;
        current_elem = current_elem->next;
        index++;
    }
    return NULL;
}

int_stack_t* int_stack_init()
{
    int_stack_t* stack = (int_stack_t*) malloc(sizeof(int_stack_t));
    stack->size = 0;
    stack->next = NULL;
    return stack;
}

void int_stack_free(int_stack_t* s)
{
    while (s->next != NULL)
    {
        int_stack_pop(s);
    }
    free(s);
}

void int_stack_push(int_stack_t* s, int value)
{
    struct int_stack_elem* new_elem = (struct int_stack_elem*) malloc(sizeof(struct int_stack_elem));
    new_elem->value = value;
    new_elem->next = s->next;
    s->next = new_elem;
    s->size++;
}

int int_stack_pop(int_stack_t* s)
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

int int_stack_contains(int_stack_t* s, int value)
{
    struct int_stack_elem* tmp = s->next;
    while (tmp != NULL)
    {
        if (tmp->value == value) return 1;
        tmp = tmp->next;
    }
    
    return 0;
}
