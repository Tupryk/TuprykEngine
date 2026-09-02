#ifndef LISTS
#define LISTS

#include <stdlib.h>

typedef struct {
    void *data;
    size_t elem_size;
    size_t size;
    size_t capacity;
} vector_t;

vector_t vector_init(size_t elem_size);
void vector_push(vector_t *v, void *element);
void vector_set(vector_t *v, size_t index, void *element);
void* vector_get(vector_t *v, size_t index);
void vector_free(vector_t *v);

struct stack_elem
{
    void* data;
    struct stack_elem* next;
    struct stack_elem* prev;
};

typedef struct
{
    size_t size;
    struct stack_elem* next;
} pstack_t;

pstack_t* stack_init();
void stack_free(pstack_t* s, void (*elem_freer)(void*));
struct stack_elem* stack_push(pstack_t* s, void* data);
void* stack_pop(pstack_t* s);
void* stack_pop_at_index(pstack_t* s, size_t i);
void* stack_pop_elem(pstack_t* s, struct stack_elem* elem);

struct int_stack_elem
{
    int value;
    struct int_stack_elem* next;
};

typedef struct
{
    size_t size;
    struct int_stack_elem* next;
} int_stack_t;

int_stack_t* int_stack_init();
void int_stack_free(int_stack_t* s);
void int_stack_push(int_stack_t* s, int value);
int int_stack_pop(int_stack_t* s);
int int_stack_contains(int_stack_t* s, int value);

#endif
