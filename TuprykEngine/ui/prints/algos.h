#ifndef ALGOS_PRINTS
#define ALGOS_PRINTS

#include "../../Algos/lists.h"

void print_int_vector(vector_t* v);
void print_int_stack(int_stack_t* stack);
void print_stack(pstack_t* stack, void (*elem_print)(void* elem));

#endif
