#ifndef LISTS
#define LISTS

#include <stdlib.h>

typedef struct {
    void *data;
    size_t elem_size;
    size_t size;
    size_t capacity;
} vector;

vector vector_create(size_t elem_size);
void vector_push(vector *v, void *element);
void* vector_get(vector *v, size_t index);
void vector_free(vector *v);

struct stack_elem
{
    void* data;
    struct stack_elem* next;
};

typedef struct
{
    size_t size;
    struct stack_elem* next;
} stack;

stack* stack_init();
void stack_free(stack* s);
void stack_push(stack* s, void* data);
void* stack_pop(stack* s);
void* stack_pop_at_index(stack* s, size_t i);

struct int_stack_elem
{
    int value;
    struct int_stack_elem* next;
};

typedef struct
{
    size_t size;
    struct int_stack_elem* next;
} int_stack;

int_stack* int_stack_init();
void int_stack_free(int_stack* s);
void int_stack_push(int_stack* s, int value);
int int_stack_pop(int_stack* s);
int int_stack_contains(int_stack* s, int value);

#endif
