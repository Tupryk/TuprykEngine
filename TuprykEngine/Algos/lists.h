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

struct int_queue
{
    int value;
    struct int_queue* next;
};

struct int_queue* queue_push(struct int_queue* queue, int value);
int queue_contains(struct int_queue* queue, int value);

#endif
