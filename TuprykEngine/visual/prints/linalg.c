#include <stdio.h>

#include "linalg.h"


void print_tensor_element(struct tensor *t, int* indices)
{
    int n = t->shape_dim-1;

    for (int i = 0; i < t->shape_dim-1; i++) {
        int following_zeros = 1;
        for (int j = i+1; j < t->shape_dim; j++) {
            if (indices[j] != 0) {
                following_zeros = 0;
                break;
            }
        }
        if (following_zeros) {
            printf("[");
        } else if (indices[n] == 0) {
            printf(" ");
        }
    }
    double value = get_tensor_value(t, indices);
    if (value >= 0) {
        printf(" ");
    }
    printf("%g", value);
    if (indices[n] != t->shape[n]-1) {
        printf(", ");
    }
    for (int i = 0; i < t->shape_dim-1; i++) {
        int following_ends = 1;
        for (int j = i+1; j < t->shape_dim; j++) {
            if (indices[j] != t->shape[j]-1) {
                following_ends = 0;
                break;
            }
        }
        if (following_ends) {
            printf("]");
        }
    }
    if (indices[n] == t->shape[n]-1) {
        printf("\n");
    }
}

void print_tensor(struct tensor *t)
{
    tensor_loop(t, print_tensor_element);
}

void print_tensor_verbose(struct tensor *t, int verbose)
{
    print_tensor(t);

    // TODO: Should maybe do this bitwise...
    if (verbose > 0)
    {
        printf("Shape: (");
        for (int i = 0; i < t->shape_dim; i++) {
            if (i == t->shape_dim-1)
            {
                printf("%d)\n", t->shape[i]);
            }
            else
            {
                printf("%d, ", t->shape[i]);
            }
        }
    }

    if (verbose > 1)
    {
        int volume = get_tensor_volume(t);
        printf("Parameters: %d\n", volume);
    }

    if (verbose > 2)
    {
        // TODO: Specify the type of the tensor, ie. diagonal, PSD, determinant, etc.
    }
    // TODO: rank of matrix
}

void print_tensor_element_flat(struct tensor *t, int* indices)
{
    int value_idx = get_tensor_value_index(t, indices);
    double value = t->values[value_idx];
    printf("%d - (", value_idx);
    for (int i = 0; i < t->shape_dim; i++)
    {
        printf("%d", indices[i]);
        if (i != t->shape_dim-1) printf(", ");
    }
    printf("): %f\n", value);
}

void print_tensor_flat(struct tensor *t)
{
    // Main tensor
    tensor_loop(t, print_tensor_element_flat);

    // Extra info
    printf("Shape: ");
    print_shape(t->shape, t->shape_dim);
    printf("\n");
    int volume = get_tensor_volume(t);
    printf("Parameters: %d\n", volume);
}

void print_shape(int* shape, int shape_dim)
{
    printf("(");
    for (int i = 0; i < shape_dim; i++)
    {
        if (i == shape_dim-1)
        {
            printf("%d)", shape[i]);
        }
        else
        {
            printf("%d, ", shape[i]);
        }
    }
}
