#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "tensor.h"
#include "../global.h"

#ifdef DEBUG
#include "../visual/prints/linalg.h"
#endif

// TODO: switch to tensors with over shape dim 2


int get_tensor_volume(struct tensor* t) {
    int volume = 1;
    for (int i = 0; i < t->shape_dim; i++) {
        volume *= t->shape[i];
    }
    return volume;
}

struct tensor* new_tensor(int* shape, int shape_dim, float* values)
{
    #ifdef DEBUG
    if (shape_dim < 2)
    {
        printf(
            "Shape dimension must be at least 2.\n- Row vectors: (1, n)\n- Column vectors: (n, 1)\n"
        );
        exit(EXIT_FAILURE);
    }
    #endif

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
    t->values = (float*) malloc(volume * sizeof(float));
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

struct tensor* tensor_copy_shape(struct tensor* t)
{
    struct tensor* t_copy = new_tensor(t->shape, t->shape_dim, NULL);
    return t_copy;
}

struct tensor* tensor_copy(struct tensor* t)
{
    struct tensor* t_copy = new_tensor(t->shape, t->shape_dim, NULL);
    for (int i = 0; i < t->volume; i++) {
        t_copy->values[i] = t->values[i];
    }
    return t_copy;
}

void tensor_transfer_values(struct tensor* to, struct tensor* from)
{
    #ifdef DEBUG
    if (!tensors_equal_shape(from, to))
    {
        printf("Tensors must have the same shape to transfer values!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    for (int i = 0; i < to->volume; i++) {
        to->values[i] = from->values[i];
    }
}

void tensor_transfer_all(struct tensor* to, struct tensor* from)
{
    // TODO: should probably implement some safety stuff here...
    free(to->shape);
    free(to->values);
    
    to->shape = (int*) malloc(from->shape_dim * sizeof(int));
    to->values = (float*) malloc(from->volume * sizeof(float));
    
    to->shape_dim = from->shape_dim;
    to->volume = from->volume;

    for (int i = 0; i < from->shape_dim; i++) {
        to->shape[i] = from->shape[i];
    }
    for (int i = 0; i < to->volume; i++) {
        to->values[i] = from->values[i];
    }
}

void tensor_free(struct tensor* t)
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

int tensors_can_be_mult(struct tensor* a, struct tensor* b)
{
    int connection_dim = a->shape_dim > 1 && b->shape_dim > 1;
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

void tensor_mult_(struct tensor* a, struct tensor* b, struct tensor* out)
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

struct tensor* tensor_mult_give(struct tensor* a, struct tensor* b)
{
    // TODO: Maybe make a nicer fix for the row vectors...
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

    tensor_mult_(a, b, out);

    return out;
}

void tensor_mult(struct tensor* a, struct tensor* b, struct tensor* out)
{
    // TODO: Make nicer
    if (a == out || b == out)
    {
        struct tensor* tmp = tensor_mult_give(a, b);

        if (a == out)
        {
            if (tensors_equal_shape(a, tmp))
                tensor_transfer_values(a, tmp);
            else
                tensor_transfer_all(a, tmp);
        }
        else
        {
            if (tensors_equal_shape(b, tmp))
                tensor_transfer_values(b, tmp);
            else
                tensor_transfer_all(b, tmp);
        }
        tensor_free(tmp);
    }
    else
    {
        tensor_mult_(a, b, out);
    }
}

void tensor_add(struct tensor* a, struct tensor* b, struct tensor* out)
{
    // TODO: Adding tensors of shape n1Xn2X...nNxaXb + aXb
    // TODO: Safety checks
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
    tensor_free(t_copy);
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
    // TODO: Make faster
    #ifdef DEBUG
    if (a->shape_dim < 2 || a->shape[a->shape_dim-1] != a->shape[a->shape_dim-2])
    {
        printf("Identity must be square!\n");
        exit(EXIT_FAILURE);
    }
    #endif
    tensor_loop(a, identify);
}

int tensors_equal_shape(struct tensor* a, struct tensor* b)
{
    // Compare tensor volumes
    if (a->volume != b->volume) return 0;
    
    // Compare tensor shapes
    if (a->shape_dim != b->shape_dim) return 0;
    for (int i = 0; i < a->shape_dim; i++)
    {
        if (a->shape[i] != b->shape[i]) return 0;
    }

    return 1;
}

int tensors_equal(struct tensor* a, struct tensor* b)
{
    // Compare tensor volumes
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
        if (fabsf(a->values[i] - b->values[i]) > PRECISION)
        {
            return 0;
        }
    }
    
    return 1;
}

void tensor_scalar_mult(struct tensor* a, float b, struct tensor* out)
{
    for (int i = 0; i < a->volume; i++)
    {
        out->values[i] = a->values[i] * b;
    }
}

struct tensor* tensor_scalar_mult_give(struct tensor* a, float b)
{
    struct tensor* out = tensor_copy(a);
    tensor_scalar_mult(a, b, out);
    return out;
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
        printf("Cannot append tensors with shapes ");
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

int tensor_lu_decomp(struct tensor* A, struct tensor* P, struct tensor* L, struct tensor* U)
{
    // TODO: Check if a decomposition can be performed. Also make faster
    // TODO: Test for vertical matrices
    #ifdef DEBUG
    if (A->shape_dim != 2)
    {
        printf("Cannot perform LU decomposition on tensor with shape ");
        print_shape(A->shape, A->shape_dim);
        printf("\n");
        exit(EXIT_FAILURE);
    }
    #endif
    
    int operator_shape[] = {A->shape[0], A->shape[0]};
    struct tensor* M = new_tensor(operator_shape, 2, NULL);
    struct tensor* P_0 = tensor_copy_shape(M);
    
    struct tensor* P_inter = tensor_copy_shape(M);
    struct tensor* L_inter = tensor_copy_shape(M);
    struct tensor* U_inter = tensor_copy_shape(A);
    
    tensor_identity(M);
    tensor_identity(P);
    tensor_identity(L);
    tensor_transfer_values(U, A);

    int row_swaps = 0;

    int rows = A->shape[0];
    int cols = A->shape[1];
    int current_col = 0;
    for (int i = 0; i < rows-1; i++)
    {
        tensor_identity(P_0);
        
        // Set the largest starting value row at the top
        int pivot_index = -1;
        while (pivot_index == -1)
        {
            for (int j = i; j < rows; j++)
            {
                if (U->values[j * cols + current_col] != 0.f)
                {
                    pivot_index = j * cols + current_col;
                    break;
                }
            }
            if (pivot_index == -1)
            {
                current_col++;
            }
        }

        int row_idx = (pivot_index - current_col) / cols;
        
        if (rows * i + row_idx != row_idx * rows + i)  // TODO: Make this nicer
        {
            P_0->values[row_idx * rows + row_idx] = 0.f;
            P_0->values[rows * i + row_idx] = 1.f;

            P_0->values[rows * i + i] = 0.f;
            P_0->values[row_idx * rows + i] = 1.f;

            row_swaps++;
            
            tensor_mult(P_0, U, U_inter);
            tensor_transfer_values(U, U_inter);
            
            tensor_mult(P, P_0, P_inter);
            tensor_transfer_values(P, P_inter);
            
            // Exchange rows
            tensor_mult(P_0, L, L_inter);
            tensor_transfer_values(L, L_inter);
            
            // Exchange columns
            tensor_mult(L, P_0, L_inter);
            tensor_transfer_values(L, L_inter);
        }

        // Gaussian elimination
        int pivot_elem_idx = i * cols + current_col;
        for (int j = i+1; j < rows; j++)
        {
            float scaler = -1.f * (U->values[j * cols + current_col] / U->values[pivot_elem_idx]);
            M->values[j * rows + i] = scaler;
        }
        tensor_mult(M, U, U_inter);
        tensor_transfer_values(U, U_inter);
        
        for (int j = i+1; j < rows; j++)
        {
            M->values[j * rows + i] *= -1.f;
        }
        tensor_mult(L, M, L_inter);
        tensor_transfer_values(L, L_inter);
        
        for (int j = i+1; j < rows; j++)
        {
            M->values[j * rows + i] = 0.f;
        }
        current_col++;
    }
    
    tensor_free(P_0);
    tensor_free(M);
    tensor_free(P_inter);
    tensor_free(L_inter);
    tensor_free(U_inter);

    return row_swaps;
}

float tensor_determinant(struct tensor* A)
{
    #ifdef DEBUG
    if (A->shape_dim != 2 || A->shape[0] != A->shape[1])
    {
        printf("Only determinants of square matrices can be computed.");
        exit(EXIT_FAILURE);
    }
    #endif

    if (A->shape[0] == 1)
    {
        return A->values[0];
    }
    
    if (A->shape[0] == 2)
    {
        return A->values[0] * A->values[3] - A->values[1] * A->values[2];
    }

    int cols = A->shape[0];

    // TODO: Might have to create a function that skips computing L and P (ie. just counts row swaps)...
    struct tensor* P = tensor_copy_shape(A);
    struct tensor* L = tensor_copy_shape(A);
    struct tensor* U = tensor_copy_shape(A);
    int row_swaps = tensor_lu_decomp(A, P, L, U);
    
    float det = row_swaps % 2 ? -1.f : 1.f;
    for (int i = 0; i < cols; i++)
    {
        det *= U->values[i * cols + i];
    }

    tensor_free(P);
    tensor_free(L);
    tensor_free(U);

    return det;
}

void tensor_inverse(struct tensor* A, struct tensor* A_inv)
{
    // TODO: Make more efficient
    #ifdef DEBUG
    if (A->shape_dim != 2 || A->shape[0] != A->shape[1])
    {
        printf("Only the inverse of square matrices can be computed.\n");
        exit(EXIT_FAILURE);
    }
    if (!tensors_equal_shape(A, A_inv))
    {
        printf("Target matrix must share the same shape as input matrix.\n");
        exit(EXIT_FAILURE);
    }
    if (tensor_determinant(A) == 0.f)
    {
        printf("Can't compute inverse of matrix with determinant 0.\n");
        exit(EXIT_FAILURE);
    }
    #endif

    if (A->shape[0] == 1)
    {
        A_inv->values[0] = 1.f / A_inv->values[0];
        return;
    }

    int cols = A->shape[0];

    // Decompose matrix
    struct tensor* P = tensor_copy_shape(A);
    struct tensor* L = tensor_copy_shape(A);
    struct tensor* U = tensor_copy_shape(A);
    tensor_lu_decomp(A, P, L, U);
    
    // Solve LY = I
    struct tensor* Y = tensor_copy_shape(A);
    for (int col = 0; col < cols; col++)
    {
        Y->values[col * cols + col] = 1.f;

        for (int row = 0; row < col; row++)
        {
            // Forward Substitution
            float sum = 0.f;

            for (int k = row; k < col; k++)
            {
                int L_idx = col * cols + k;
                int Y_idx = k * cols + row;
                sum -= L->values[L_idx] * Y->values[Y_idx];
            }
            
            Y->values[col * cols + row] = sum;
        }
    }
    
    // Solve UA^-1 = Y
    for (int row = cols-1; row >= 0; row--)
    {
        for (int col = cols-1; col >= 0; col--)
        {
            // Backward Substitution
            float sum = 0.f;

            for (int k = col+1; k < cols; k++)
            {
                int U_idx = col * cols + k;
                int X_idx = k * cols + row;
                sum += U->values[U_idx] * A_inv->values[X_idx];
            }
            
            int A_inv_idx = col * cols + row;
            sum = Y->values[A_inv_idx] - sum;
            A_inv->values[A_inv_idx] = sum / U->values[col * cols + col];
        }
    }

    tensor_free(P);
    tensor_free(L);
    tensor_free(U);
    tensor_free(Y);
}

struct tensor* tensor_inverse_give(struct tensor* A)
{
    struct tensor* A_inv = tensor_copy_shape(A);
    tensor_inverse(A, A_inv);
    return A_inv;
}

float tensor_vec_magnitude(struct tensor* t)
{
    // TODO: Make this work for tensors with axis as input like numpy's np.linalg.norm(...).
    #ifdef DEBUG
    if (!(t->shape_dim == 1 || (t->shape_dim == 2 && (t->shape[0] == 1 || t->shape[1] == 1))))
    {
        printf("Function currently only works for vectors such that t->shape_dim = 1.\n");
        exit(EXIT_FAILURE);
    }
    #endif
    float magnitude = 0.f;
    for (int i = 0; i < t->volume; i++) {
        magnitude += t->values[i] * t->values[i];
    }
    magnitude = sqrt(magnitude);
    return magnitude;
}

void tensor_flatten(struct tensor* t)
{
    free(t->shape);
    
    t->shape = t->shape = (int*) malloc(sizeof(int) * 2);
    t->shape[0] = t->volume;
    t->shape_dim = 2;
}

float tensor_max(struct tensor* t)
{
    float out = t->values[0];
    for (int i = 1; i < t->volume; i++)
    {
        if (out < t->values[i])
        {
            out = t->values[i];
        }
    }
    return out;
}

float tensor_min(struct tensor* t)
{
    float out = t->values[0];
    for (int i = 1; i < t->volume; i++)
    {
        if (out > t->values[i])
        {
            out = t->values[i];
        }
    }
    return out;
}

struct tensor* tensor_slice(struct tensor* t, int idx)
{
    // TODO: test properly and make nicer
    int new_shape_dim = t->shape_dim == 2 ? 2 : t->shape_dim-1;
    int new_shape[new_shape_dim];
    int step = 1;
    for (int i = 1; i < t->shape_dim; i++)
    {
        new_shape[i-1] = t->shape[i];
        step *= t->shape[i];
    }
    if (t->shape_dim == 2) new_shape[1] = 1;
    struct tensor* out = new_tensor(new_shape, new_shape_dim, NULL);

    int out_idx = 0;
    for (int i = idx; i < t->volume; i+=step)
    {
        out->values[out_idx] = t->values[i];
        out_idx++;
    }

    return out;
}
