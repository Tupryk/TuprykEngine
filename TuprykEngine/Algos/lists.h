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

struct int_queue
{
    int value;
    struct int_queue* next;
};

struct int_queue* queue_add_val(struct int_queue* queue, int value);
int value_in_queue(struct int_queue* queue, int value);

#endif
