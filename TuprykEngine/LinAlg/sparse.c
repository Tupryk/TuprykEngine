#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "sparse.h"


struct sparse* new_sparse(int* shape, int shape_dim)
{
    struct sparse* st = (struct sparse*) malloc(sizeof(struct sparse));

    st->shape_dim = shape_dim;
    st->shape = (int*) malloc(shape_dim * sizeof(int));
    for (int i = 0; i < shape_dim; i++)
    {
        #ifdef DEBUG
        if (shape[i] <= 0) {
            fprintf(stderr, "Error: invalid tensor shape\n");
            exit(EXIT_FAILURE);
        }
        #endif
        st->shape[i] = shape[i];
    }
    
    st->value_count = 0;
    st->volume = 1;
    for (int i = 0; i < st->shape_dim; i++) {
        st->volume *= st->shape[i];
    };
    st->values = NULL;
    st->indices = NULL;

    return st;
}

struct sparse* sparse_from_tensor(struct tensor* t)
{
    struct sparse* st = new_sparse(t->shape, t->shape_dim);

    // TODO: Make more efficient
    for (int i = 0; i < t->volume; i++)
    {
        if (fabsf(t->values[i]) > SPARSE_PRECISION)
        {
            sparse_insert(st, t->values[i], i);
        }
    }

    return st;
}

struct tensor* tensor_from_sparse(struct sparse* st)
{
    struct tensor* t = new_tensor(st->shape, st->shape_dim, NULL);

    for (int i = 0; i < st->value_count; i++)
    {
        t->values[st->indices[i]] = st->values[i];
    }

    return t;
}

void sparse_free(struct sparse* st)
{
    free(st->shape);
    if (st->values != NULL)
    {
        free(st->values);
        free(st->indices);
    }
    free(st);
}

void sparse_insert(struct sparse* st, float value, int index)
{
    // TODO: Make more efficient
    st->value_count++;

    int* new_indices = (int*) malloc(sizeof(int) * st->value_count);
    float* new_values = (float*) malloc(sizeof(float) * st->value_count);
    
    if (st->values == NULL)
    {
        new_indices[0] = index;
        new_values[0] = value;
    }
    else
    {
        int switched = 0;
        for (int i = 0; i < st->value_count - 1; i++)
        {
            if (st->indices[i] < index)
            {
                new_indices[i] = st->indices[i];
                new_values[i] = st->values[i];
            }
            else if (st->indices[i] > index)
            {
                if (switched)
                {
                    new_indices[i+1] = st->indices[i];
                    new_values[i+1] = st->values[i];
                }
                else
                {
                    new_indices[i] = index;
                    new_values[i] = value;
                    switched = 1;
                    i--;
                }
            }
            else
            {
                st->value_count--;
                st->values[i] = value;
                return;
            }
        }

        if (!switched)
        {
            int i = st->value_count-1;
            new_indices[i] = index;
            new_values[i] = value;
        }
        
        free(st->indices);
        free(st->values);
    }

    st->indices = new_indices;
    st->values = new_values;
}
