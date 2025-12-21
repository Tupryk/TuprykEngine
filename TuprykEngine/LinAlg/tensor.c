#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tensor.h"
#include "../global.h"

#ifdef DEBUG
#include "../visual/prints/linalg.h"
#endif


int get_tensor_volume(struct tensor* t) {
    int volume = 1;
    for (int i = 0; i < t->shape_dim; i++) {
        volume *= t->shape[i];
    }
    return volume;
}

struct tensor* new_tensor(int* shape, int shape_dim, TYPE* values)
{
    struct tensor* t = (struct tensor*) malloc(sizeof(struct tensor));
    t->shape_dim = shape_dim;
    t->shape = (int*) malloc(shape_dim * sizeof(int));
    for (int i = 0; i < shape_dim; i++)
    {
        #ifdef DEBUG
        if (shape[i] <= 0) {
            fprintf(stderr, "Error: invalid tensor shape\n");
            exit(EXIT_FAILURE);
        }
        #endif
        t->shape[i] = shape[i];
    }
    
    int volume = get_tensor_volume(t);
    t->values = (TYPE*) malloc(volume * sizeof(TYPE));
    t->volume = volume;

    if (values != NULL)
    {
        for (int i = 0; i < volume; i++)
            t->values[i] = values[i];
    }
    else
    {
        for (int i = 0; i < volume; i++)
            t->values[i] = 0.0;
    }
    // TODO: Add a universal allocated memory list so that everything can be removed from memory at once at the end of each program.
    return t;
}

struct tensor* tensor_copy(struct tensor* t)
{
    struct tensor* t_copy = new_tensor(t->shape, t->shape_dim, NULL);
    for (int i = 0; i < t->volume; i++) {
        t_copy->values[i] = t->values[i];
    }
    return t_copy;
}

void free_tensor(struct tensor* t)
{
    // TODO: could maybe make this take a list of tensors as input.
    free(t->values);
    free(t->shape);
    free(t);
}

int get_tensor_value_index(struct tensor* t, int* indices)
{
    int index = indices[t->shape_dim - 1];
    int sum_of_dims = 1;

    for (int i = t->shape_dim - 2; i >= 0; i--) {
        sum_of_dims *= t->shape[i+1];
        index += indices[i] * sum_of_dims;
    }
    return index;
}

double get_tensor_value(struct tensor* t, int* indices)
{
    int index = get_tensor_value_index(t, indices);
    return t->values[index];
}

void fill_random(struct tensor* t)
{
    for (int i = 0; i < t->volume; i++) {
        t->values[i] = ((double) rand() / RAND_MAX) * 2.0 -0.5;
    }
}

void fill_const(struct tensor* t, double num)
{
    for (int i = 0; i < t->volume; i++) {
        t->values[i] = num;
    }
}

void recursive_tensor_mult(struct tensor* a, struct tensor* b, struct tensor* out, int dim_idx)
{
    if (dim_idx == a->shape_dim-1) {
        // Dot Product
    } else if (dim_idx == a->shape_dim-2) {
        // Matrix Products
    } else {
        for (int i = 0; i < a->shape[dim_idx]; i++) {
            recursive_tensor_mult(a, b, out, dim_idx+1);
        }
    }
}

int tensors_can_be_mult(struct tensor* a, struct tensor* b)
{
    int connection_dim = a->shape_dim > 1 || b->shape_dim > 1;
    if (b->shape_dim > 1) {
        connection_dim = a->shape[a->shape_dim-1] == b->shape[b->shape_dim-2];
    }
    int prev_equal = 1;
    int min_dim = a->shape_dim < b->shape_dim ? a->shape_dim : b->shape_dim;
    for (int i = min_dim-3; i >= 0; i--)
    {
        int ai = a->shape_dim < b->shape_dim ? i : a->shape_dim - min_dim + i;
        int bi = a->shape_dim < b->shape_dim ? b->shape_dim - min_dim + i : i;
        if (a->shape[ai] != b->shape[bi] && a->shape[ai] != 1 && b->shape[bi] != 1) {
            prev_equal = 0;
            break;
        }
    }
    return connection_dim && prev_equal;
}

struct tensor* tensor_mult_give(struct tensor* a, struct tensor* b)
{
    int out_shape_dim = a->shape_dim < b->shape_dim ? b->shape_dim : a->shape_dim;
    int out_shape[out_shape_dim];
    
    int* bigger_shape = a->shape_dim < b->shape_dim ? b->shape : a->shape;
    for (int i = 0; i < out_shape_dim; i++)
    {
        out_shape[i] = bigger_shape[i];
    }
    
    out_shape[out_shape_dim-1] = b->shape[b->shape_dim-1];
    out_shape[out_shape_dim-2] = a->shape_dim > 1 ? a->shape[a->shape_dim-2] : 1 ;

    struct tensor* out = new_tensor(out_shape, out_shape_dim, NULL);

    tensor_mult(a, b, out);

    return out;
}

void tensor_mult(struct tensor* a, struct tensor* b, struct tensor* out)
{
    // Check if tensor operation is possible
    #ifdef DEBUG
    if (!tensors_can_be_mult(a, b)) {
        fprintf(stderr, "Error: invalid tensor shapes: a -> ");
        print_shape(a->shape, a->shape_dim);
        printf(" ; b -> ");
        print_shape(b->shape, b->shape_dim);
        printf("\n");
        exit(EXIT_FAILURE);
    }
    #endif

    // Initialize loop variables
    int out_mat_area = out->shape[out->shape_dim-1] * out->shape[out->shape_dim-2];
    int a_col_len = a->shape[a->shape_dim-1];
    int a_vol = a->volume;
    int b_col_len = b->shape[b->shape_dim-1];
    int b_mat_area = b->shape[b->shape_dim-1] * b->shape[b->shape_dim-2];
    int b_step = 1;
    for (int i = 0; i < b->shape_dim-2; i++)
    {
        b_step *= b->shape[i];
    }
    
    // Execute tensor operation
    for (int i = 0; i < out->volume; i++)
    {
        out->values[i] = 0.0;
        for (int j = 0; j < a_col_len; j++)
        {
            int ai = (i / b_col_len * a_col_len + j) % a_vol;
            
            int offset = ((i / out_mat_area) % b_step) * b_mat_area;
            int bi = i % b_col_len + offset + (j * b_col_len);

            out->values[i] += a->values[ai] * b->values[bi];
        }
    }
}

void tensor_add(struct tensor* a, struct tensor* b, struct tensor* out)
{
    for (int i = 0; i < a->volume; i++) {
        out->values[i] = a->values[i] + b->values[i];
    }
}

void tensor_sub(struct tensor* a, struct tensor* b, struct tensor* out)
{
    for (int i = 0; i < a->volume; i++) {
        out->values[i] = a->values[i] - b->values[i];
    }
}

void tensor_loop(struct tensor* t, void (*func)(struct tensor*, int*))
{
    int counts[t->shape_dim];
    for (int i = 0; i < t->shape_dim; i++) {
        counts[i] = 0;
    }

    int n = t->shape_dim-1;
    while (counts[0] < t->shape[0])
    {
        func(t, counts);

        counts[n]++;
        for (int i = n; i > 0; i--) {
            if (counts[i] >= t->shape[i]) {
                counts[i] = 0;
                counts[i-1]++;
            } else {
                break;
            }
        }
    }
}

void tensor_transpose(struct tensor* t)
{
    // Special case: column vector to row vector
    if (t->shape_dim == 1)
    {
        int vec_dim = t->shape[0];
        free(t->shape);
        
        t->shape_dim = 2;
        t->shape = (int*) malloc(2 * sizeof(int));
        t->shape[0] = vec_dim;
        t->shape[1] = 1;
        return;
    }
    
    // Create a copy of the vector to store the original tensor information
    struct tensor* t_copy = tensor_copy(t);
    for (int i = 0; i < t->shape_dim; i++) {
        t->shape[i] = t_copy->shape[t->shape_dim-1-i];
    }

    // Initialize loop variables
    int mods[t->shape_dim];
    int divs[t->shape_dim];
    int mults[t->shape_dim];
    for (int i = 0; i < t->shape_dim; i++)
    {
        mods[i] = 1;
        for (int j = t->shape_dim-1; j > t->shape_dim-2-i; j--)
        {
            mods[i] *= t->shape[j];
        }

        divs[i] = 1;
        for (int j = t->shape_dim-1; j > t->shape_dim-1-i; j--)
        {
            divs[i] *= t->shape[j];
        }

        mults[i] = 1;
        for (int j = 0; j < t->shape_dim-1-i; j++)
        {
            mults[i] *= t->shape[j];
        }
    }

    // Perform tensor transpose
    for (int i = 0; i < t->volume; i++)
    {
        int tci = 0;

        for (int j = 0; j < t->shape_dim; j++)
        {
            tci += i % mods[j] / divs[j] * mults[j];
        }

        t->values[i] = t_copy->values[tci];
    }
    free_tensor(t_copy);
}

void identify(struct tensor* a, int* counts)
{
    int equal = counts[a->shape_dim-1] == counts[a->shape_dim-2];
    int index = get_tensor_value_index(a, counts);

    if (equal) {
        a->values[index] = 1.0;
    } else {
        a->values[index] = 0.0;
    }
}

void tensor_identity(struct tensor* a)
{
    #ifdef DEBUG
    if (a->shape_dim < 2 || a->shape[a->shape_dim-1] != a->shape[a->shape_dim-2])
    {
        printf("Identity must be square!");
        exit(EXIT_FAILURE);
    }
    #endif
    tensor_loop(a, identify);
}

int tensors_equal(struct tensor* a, struct tensor* b)
{
    if (a->volume != b->volume) return 0;
    
    // Compare tensor shapes
    if (a->shape_dim != b->shape_dim) return 0;
    for (int i = 0; i < a->shape_dim; i++)
    {
        if (a->shape[i] != b->shape[i]) return 0;
    }

    // Compare tensor values
    for (int i = 0; i < a->volume; i++)
    {
        if (fabs(a->values[i] - b->values[i]) > PRECISION)
        {
            return 0;
        }
    }
    
    return 1;
}

void tensor_scalar_mult(struct tensor* a, TYPE b, struct tensor* out)
{
    for (int i = 0; i < a->volume; i++)
    {
        out->values[i] = a->values[i] * b;
    }
}

int tensor_is_square(struct tensor* t)
{
    return t->shape_dim >= 2 && t->shape[t->shape_dim-1] == t->shape[t->shape_dim-2];
}

void tensor_reshape(struct tensor* t, int* shape, int shape_dim)
{
    #ifdef DEBUG
    int v0 = t->volume;
    int v1 = 1;
    for (int i = 0; i < shape_dim; i++)
    {
        v1 *= shape[i];
    }
    if (shape_dim == 0 || v0 != v1)
    {
        printf("Tensor can not be reshaped from ");
        print_shape(t->shape, t->shape_dim);
        printf(" to ");
        print_shape(shape, shape_dim);
        printf(" volumes don't match!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    if (shape_dim != t->shape_dim)
    {
        t->shape_dim = shape_dim;
        free(t->shape);
        t->shape = (int*) malloc(shape_dim * sizeof(int));
    }

    for (int i = 0; i < shape_dim; i++)
    {
        t->shape[i] = shape[i];
    }
}

void tensor_unsqueeze(struct tensor* t, int axis)
{
    #ifdef DEBUG
    if (axis > t->shape_dim)
    {
        printf("Can't unsqueeze tensor with shape ");
        print_shape(t->shape, t->shape_dim);
        printf(" on axis %d.\n", axis);
        exit(EXIT_FAILURE);
    }
    #endif

    if (axis == -1) axis = t->shape_dim-1;

    t->shape_dim++;

    if (axis == -1) axis = t->shape_dim-1;

    int new_shape[t->shape_dim];
    for (int i = axis; i < t->shape_dim; i++)
    {
        if (i < axis)
        {
            new_shape[i] = t->shape[i];
        }
        else if (i > axis)
        {
            new_shape[i] = t->shape[i-1];
        }
        else if (i == axis)
        {
            new_shape[i] = 1;
        }
    }

    free(t->shape);
    t->shape = (int*) malloc(t->shape_dim * sizeof(int));

    for (int i = 0; i < t->shape_dim; i++)
    {
        t->shape[i] = new_shape[i];
    }
}

void tensor_squeeze(struct tensor* t, int axis)
{
    #ifdef DEBUG
    if (t->shape_dim == 1 || axis >= t->shape_dim || t->shape[axis] != 1)
    {
        printf("Can't squeeze tensor with shape ");
        print_shape(t->shape, t->shape_dim);
        printf(" on axis %d.\n", axis);
        exit(EXIT_FAILURE);
    }
    #endif

    t->shape_dim--;

    int new_shape[t->shape_dim];
    for (int i = axis; i < t->shape_dim; i++)
    {
        new_shape[i] = i >= axis ? t->shape[i+1] : t->shape[i];
    }

    free(t->shape);
    t->shape = (int*) malloc(t->shape_dim * sizeof(int));

    for (int i = 0; i < t->shape_dim; i++)
    {
        t->shape[i] = new_shape[i];
    }
}

struct tensor* tensor_append(struct tensor* a, struct tensor* b, int axis)
{
    if (axis == -1) {
        axis = a->shape_dim-1;
    }

    #ifdef DEBUG
    int feasible = 0;
    if (a->shape_dim == b->shape_dim && axis < a->shape_dim)
    {
        feasible = 1;
        for (int i = 0; i < a->shape_dim; i++)
        {
            if (i != axis && a->shape[i] != b->shape[i])
            {
                feasible = 0;
                break;
            }
        }
    }
    if (!feasible)
    {
        printf("Cannot append tensors with shapes (");
        print_shape(a->shape, a->shape_dim);
        printf(" and ");
        print_shape(b->shape, b->shape_dim);
        printf(" on axis %d.\n", axis);
        exit(EXIT_FAILURE);
    }
    #endif

    int out_shape[a->shape_dim];
    for (int i = 0; i < a->shape_dim; i++)
    {
        out_shape[i] = i == axis ? a->shape[i] + b->shape[i] : a->shape[i];
    }

    // Why does this work??????
    struct tensor* out = new_tensor(out_shape, a->shape_dim, NULL);
    for (int i = 0; i < out->volume; i++)
    {
        if (i < a->volume) {
            out->values[i] = a->values[i];
        } else {
            out->values[i] = b->values[i - a->volume];
        }
    }

    return out;
}
