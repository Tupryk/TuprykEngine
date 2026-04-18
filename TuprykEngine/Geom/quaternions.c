#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "quaternions.h"


void quaternion_from_euler(float x, float y, float z, float* out)
{
    float cx = cosf(x * 0.5f);
    float sx = sinf(x * 0.5f);
    float cy = cosf(y * 0.5f);
    float sy = sinf(y * 0.5f);
    float cz = cosf(z * 0.5f);
    float sz = sinf(z * 0.5f);

    out[0] = cx*cy*cz - sx*sy*sz;
    out[1] = sx*cy*cz + cx*sy*sz;
    out[2] = cx*sy*cz - sx*cy*sz;
    out[3] = cx*cy*sz + sx*sy*cz;
}

void quaternion_x(float phi, float* out)
{
    out[0] = cosf(phi * 0.5f);
    out[1] = sinf(phi * 0.5f);
    out[2] = 0.f;
    out[3] = 0.f;
}

void quaternion_y(float phi, float* out)
{
    out[0] = cosf(phi * 0.5f);
    out[1] = 0.f;
    out[2] = sinf(phi * 0.5f);
    out[3] = 0.f;
}

void quaternion_z(float phi, float* out)
{
    out[0] = cosf(phi * 0.5f);
    out[1] = 0.f;
    out[2] = 0.f;
    out[3] = sinf(phi * 0.5f);
}

void quaternion_to_matrix(float* q, tensor* out)
{
    #ifdef DEBUG
    if (out->shape_dim != 2 || out->shape[0] != 3 || out->shape[1] != 3)
    {
        printf("Invalid rotation matrix shape!\n");
        exit(EXIT_FAILURE);
    }
    #endif

    float w = q[0];
    float x = q[1];
    float y = q[2];
    float z = q[3];
    float x2 = x * x;
    float y2 = y * y;
    float z2 = z * z;

    out->values[0] = 1 - 2.f*y2 - 2.f*z2;
    out->values[1] = 2.f*x*y - 2.f*z*w;
    out->values[2] = 2.f*x*z - 2.f*y*w;

    out->values[3] = 2.f*x*y - 2.f*z*w;
    out->values[4] = 1 - 2.f*x2 - 2.f*z2;
    out->values[5] = 2.f*y*z - 2.f*x*w;

    out->values[6] = 2.f*x*z - 2.f*y*w;
    out->values[7] = 2.f*y*z - 2.f*x*w;
    out->values[8] = 1 - 2.f*y2 - 2.f*y2;
}

tensor* quaternion_to_matrix_give(float* q)
{
    int shape[] = {3, 3};
    tensor* out = new_tensor(shape, 2, NULL);
    quaternion_to_matrix(q, out);
    return out;
}

void quaternion_conjugate(float* q, float* out)
{
    out[0] =  q[0];
    out[1] = -q[1];
    out[2] = -q[2];
    out[3] = -q[3];
}

void quaternion_product(float* a, float* b, float* out)
{
    out[0] = a[0]*b[0] - a[1]*b[1] - a[2]*b[2] - a[3]*b[3];
    out[1] = a[1]*b[0] + a[0]*b[1] - a[3]*b[2] + a[2]*b[3];
    out[2] = a[2]*b[0] + a[3]*b[1] + a[0]*b[2] - a[1]*b[3];
    out[3] = a[3]*b[0] - a[2]*b[1] + a[1]*b[2] + a[0]*b[3];
}

void quaternion_rotate_point(float* q, float* p, float* out)
{
    // TODO: Use optimized version
    float p_q[4] = {0.f, p[0], p[1], p[2]};
    float q_inv[4];
    float tmp[4];

    quaternion_conjugate(q, q_inv);
    quaternion_product(q, p_q, tmp);
    quaternion_product(tmp, q_inv, p_q);
    
    out[0] = p_q[1];
    out[1] = p_q[2];
    out[2] = p_q[3];
}
