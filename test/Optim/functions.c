#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "../../TuprykEngine/Algos/utils.h"
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/visual/prints/linalg.h"
#include "../../TuprykEngine/Optim/constrained/nlp.h"


nlp_t* get_basic_nlp(int eq_count, int ineq_count, int order)
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = eq_count;
    nlp->ineq_count = ineq_count;
    
    nlp->f = NULL;
    nlp->eq = NULL;
    nlp->ineq = NULL;
    
    nlp->delta_f = NULL;
    nlp->delta_eq = NULL;
    nlp->delta_ineq = NULL;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;
    
    if (eq_count > 0) nlp->eq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->eq_count);
    if (ineq_count > 0) nlp->ineq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->ineq_count);
    
    if (order > 0)
    {
        if (eq_count > 0) nlp->delta_eq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->eq_count);
        if (ineq_count > 0) nlp->delta_ineq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->ineq_count);
    }
    
    if (order > 1)
    {
        if (eq_count > 0) nlp->delta2_eq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->eq_count);
        if (ineq_count > 0) nlp->delta2_ineq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->ineq_count);
    }

    return nlp;
}

void quadratic_init()
{
    int shape_A[] = {2, 2};
    float values_A[] = {
        1, 0,
        0, 1
    };
    q.A = new_tensor(shape_A, 2, values_A);

    int shape_b[] = {1, 2};
    float values_b[] = {
        -2,
        4
    };
    q.b = new_tensor(shape_b, 2, values_b);

    q.c = 5.f;
}

void quadratic_free()
{
    tensor_free(q.A);
    tensor_free(q.b);
}

float quadratic_eval(tensor* x)
{
    tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);

    tensor* dot0 = tensor_mult_give(q.A, x);
    tensor* dot1 = tensor_mult_give(x_T, dot0);
    tensor* dot2 = tensor_mult_give(q.b, x);
    
    float out = 0.5 * dot1->values[0] + dot2->values[0] + q.c;
    
    tensor_free(x_T);
    tensor_free(dot0);
    tensor_free(dot1);
    tensor_free(dot2);

    return out;
}

void quadratic_eval2(tensor* x, tensor* out)
{
    tensor* b_T = tensor_copy(q.b);
    tensor_transpose(b_T);

    tensor_mult(q.A, x, out);
    tensor_add(out, q.b, out);
    
    tensor_free(b_T);
}

void quadratic_eval3(tensor* x, tensor* out)
{
    tensor_transfer_values(out, q.A);
}

nlp_t* get_nlp0()
{
    nlp_t* nlp = get_basic_nlp(0, 0, 2);
    nlp->f = quadratic_eval;
    nlp->delta_f = quadratic_eval2;
    nlp->delta2_f = quadratic_eval3;
    return nlp;
}

float zero_eval(tensor* x) { return 0.f; }
void zero_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = 0.f; }

float squared_eval(tensor* x) { return .5f * ((x->values[0] - .5f)*(x->values[0] - .5f) + (x->values[1] + .5f)*(x->values[1] + .5f)) * 10.f; }
void squared_eval2(tensor* x, tensor* out) { out->values[0] = (x->values[0] - .5f) * 10.f; out->values[1] = (x->values[1] + .5f) * 10.f; }

float side1_eval(tensor* x) { return x->values[0] - 0.5; }
void side1_eval2(tensor* x, tensor* out) { out->values[0] = 1.f; out->values[1] = 0.f; }

float side2_eval(tensor* x) { return -x->values[0] - 0.5; }
void side2_eval2(tensor* x, tensor* out) { out->values[0] = -1.f; out->values[1] = 0.f; }

float side3_eval(tensor* x) { return -x->values[1] - 0.5; }
void side3_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = -1.f; }

float side4_eval(tensor* x) { return x->values[1] - 0.5; }
void side4_eval2(tensor* x, tensor* out) { out->values[0] = 0.f; out->values[1] = 1.f; }

nlp_t* get_nlp1()
{
    nlp_t* nlp = (nlp_t*) malloc(sizeof(nlp_t));
    nlp->eq_count = 0;
    nlp->ineq_count = 4;
    
    nlp->f = squared_eval;
    nlp->eq = NULL;
    nlp->ineq = (scalar_fn*) malloc(sizeof(scalar_fn) * nlp->ineq_count);
    nlp->ineq[0] = side1_eval;
    nlp->ineq[1] = side2_eval;
    nlp->ineq[2] = side3_eval;
    nlp->ineq[3] = side4_eval;
    
    nlp->delta_f = squared_eval2;
    nlp->delta_eq = NULL;
    nlp->delta_ineq = (vector_fn*) malloc(sizeof(vector_fn) * nlp->ineq_count);
    nlp->delta_ineq[0] = side1_eval2;
    nlp->delta_ineq[1] = side2_eval2;
    nlp->delta_ineq[2] = side3_eval2;
    nlp->delta_ineq[3] = side4_eval2;
    
    nlp->delta2_f = NULL;
    nlp->delta2_eq = NULL;
    nlp->delta2_ineq = NULL;

    return nlp;
}

float circle_eval(tensor* x) { return vector_squared_norm(x) - 0.5; }
void circle_eval2(tensor* x, tensor* out) { tensor_scalar_mult(x, 2, out); }

nlp_t* get_nlp2()
{
    nlp_t* nlp = get_basic_nlp(0, 1, 1);
    nlp->f = zero_eval;
    nlp->delta_f = zero_eval2;
    nlp->ineq[0] = circle_eval;
    nlp->delta_ineq[0] = circle_eval2;
    return nlp;
}

nlp_t* get_nlp3()
{
    nlp_t* nlp = get_basic_nlp(1, 0, 1);
    nlp->f = zero_eval;
    nlp->delta_f = zero_eval2;
    nlp->eq[0] = circle_eval;
    nlp->delta_eq[0] = circle_eval2;
    return nlp;
}

nlp_t* get_nlp4()
{    
    nlp_t* nlp = get_basic_nlp(1, 0, 1);
    nlp->f = zero_eval;
    nlp->delta_f = zero_eval2;
    nlp->eq[0] = side1_eval;
    nlp->delta_eq[0] = side1_eval2;
    return nlp;
}

float disk_1_radius = 0.2f;
float disk1_center_values[] = {0.5f, 0.15f};

float disk_2_radius = 0.02f;
float disk2_center_values[] = {-0.5f, -0.5f};

float two_disks_eval(tensor* x)
{
    tensor* x_delta = tensor_copy_shape(x);

    tensor* disk1_center = new_tensor_vector(2, disk1_center_values);
    tensor* disk2_center = new_tensor_vector(2, disk2_center_values);

    tensor_sub(x, disk1_center, x_delta);
    float disk1 = vector_squared_norm(x_delta) - disk_1_radius;
    tensor_free(disk1_center);

    tensor_sub(x, disk2_center, x_delta);
    float disk2 = vector_squared_norm(x_delta) - disk_2_radius;
    tensor_free(disk2_center);

    tensor_free(x_delta);

    return min(disk1, disk2);
}

void two_disks_eval2(tensor* x, tensor* out)
{
    tensor* x_delta1 = tensor_copy_shape(x);
    tensor* x_delta2 = tensor_copy_shape(x);

    tensor* disk1_center = new_tensor_vector(2, disk1_center_values);
    tensor* disk2_center = new_tensor_vector(2, disk2_center_values);

    tensor_sub(x, disk1_center, x_delta1);
    tensor_free(disk1_center);
    float disk1 = vector_squared_norm(x_delta1) - disk_1_radius;

    tensor_sub(x, disk2_center, x_delta2);
    tensor_free(disk2_center);
    float disk2 = vector_squared_norm(x_delta2) - disk_2_radius;

    if (disk1 < disk2) tensor_scalar_mult(x_delta1, 2, out);
    else tensor_scalar_mult(x_delta2, 2, out);

    tensor_free(x_delta1);
    tensor_free(x_delta2);
}

nlp_t* get_nlp_two_disks()
{
    nlp_t* nlp = get_basic_nlp(0, 1, 1);
    nlp->f = zero_eval;
    nlp->delta_f = zero_eval2;
    nlp->ineq[0] = two_disks_eval;
    nlp->delta_ineq[0] = two_disks_eval2;
    return nlp;
}

float swiss_roll_eval(tensor* x)
{
    float t_max = 5.0f * M_PI;
    float px = x->values[0];
    float py = x->values[1];
    float r     = vector_norm(x) * t_max;
    float theta = atan2f(py, px);
    float n     = roundf((r - theta) / (2.0f * M_PI));
    theta      += n * 2.0f * M_PI;

    float spiral = r - theta;

    float cap = r - t_max;

    return fmaxf(spiral, cap);
}

void swiss_roll_eval2(tensor* x, tensor* out)
{
    float t_max = 5.0f * M_PI;
    float px = x->values[0];
    float py = x->values[1];
    float r2 = px*px + py*py;
    float theta = atan2f(py, px);
    float r  = sqrtf(r2) + 1e-8f;
    float r_scaled = r * t_max;

    float dr_dx = t_max * px / r;
    float dr_dy = t_max * py / r;

    float dtheta_dx = -py / r2;
    float dtheta_dy =  px / r2;

    float spiral = r_scaled - theta;
    float cap    = r_scaled - t_max;

    if (cap > spiral)
    {
        out->values[0] = dr_dx;
        out->values[1] = dr_dy;
    }
    else
    {
        out->values[0] = dr_dx - dtheta_dx;
        out->values[1] = dr_dy - dtheta_dy;
    }
}

nlp_t* get_nlp_swiss_roll()
{
    nlp_t* nlp = get_basic_nlp(0, 1, 1);
    nlp->f = zero_eval;
    nlp->delta_f = zero_eval2;
    
    // nlp->eq[0] = swiss_roll_eval;
    // nlp->delta_eq[0] = swiss_roll_eval2;

    nlp->ineq[0] = swiss_roll_eval;
    nlp->delta_ineq[0] = swiss_roll_eval2;
    return nlp;
}

float gaussian(tensor* x)
{
    float std = .25f;

    float norm2 = vector_squared_norm(x);

    return 1.f / sqrtf(2.f * M_PI * std * std) * expf(-norm2 / (2.f * std * std));
}
