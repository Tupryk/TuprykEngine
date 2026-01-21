#ifndef SPARSE
#define SPARSE


struct value_chain
{
    int length;
    struct value_chain_link* start;
    struct value_chain_link* end;
};

struct value_chain_link
{
    float value;
    struct value_chain_link* next;
    struct value_chain_link* prev;
};

struct index_chain
{
    int length;
    struct value_chain_link* start;
    struct value_chain_link* end;
};

struct index_chain_link
{
    int index;
    struct index_chain_link* next;
    struct index_chain_link* prev;
};

int index_chain_insert(struct index_chain* ic);
void value_chain_insert(struct index_chain* vc, int index);

struct sparse_tensor
{
    int* shape;
    int shape_dim;
    int volume;
    struct value_chain* values;
    struct index_chain* indices;
};

struct sparse_tensor* new_sparse_tensor(int* shape, int shape_dim);
struct sparse_tensor* sparse_from_tensor(struct tensor* t, float precision);
struct tensor* tensor_from_sparse(struct sparse_tensor* st);
void sparse_tensor_free(struct sparse_tensor* st);
void sparse_tensor_insert(struct sparse_tensor* float value, int index);

#endif
