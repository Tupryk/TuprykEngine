#include "sparse.h"


int index_chain_insert(struct index_chain* ic, int new_index)
{

}

void value_chain_insert(struct index_chain* vc, float value, int index)
{

}

struct sparse_tensor* new_sparse_tensor(int* shape, int shape_dim)
{
    struct sparse_tensor* st = (struct sparse_tensor*) malloc(sizeof(struct sparse_tensor));

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
    
    st->volume = 1;
    for (int i = 0; i < st->shape_dim; i++) {
        st->volume *= st->shape[i];
    };

    st->indices = (struct index_chain*) malloc(sizeof(struct index_chain));
    st->indices->start = NULL;
    st->indices->end = NULL;

    st->values = (struct value_chain*) malloc(sizeof(struct value_chain));
    st->values->start = NULL;
    st->values->end = NULL;
}

struct sparse_tensor* sparse_from_tensor(struct tensor* t, float precision)
{
    struct sparse_tensor* st = new_sparse_tensor(t->shape, t->shape_dim);

    for (int i = 0; i < t->volume; i++)
    {
        if (fabsf(t->values[i]) > precision)
        {
            sparse_tensor_insert(st, t->values[i], i);
        }
    }
}

struct tensor* tensor_from_sparse(struct sparse_tensor* st)
{

}

void sparse_tensor_free(struct sparse_tensor* st)
{

}

void sparse_tensor_insert(struct sparse_tensor* st, float value, int index)
{

}
