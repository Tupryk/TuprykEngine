#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void quaternion_to_matrix(float* q, tensor_t* out)
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

tensor_t* quaternion_to_matrix_give(float* q)
{
    int shape[] = {3, 3};
    tensor_t* out = new_tensor(shape, 2, NULL);
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

void quaternion_normalize(float* q)
{
    float norm = sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3]);

    if (norm == 0.f)
    {
        q[0] = 1.f;
        q[1] = 0.f;
        q[2] = 0.f;
        q[3] = 0.f;
    }
    else
    {
        q[0] /= norm;
        q[1] /= norm;
        q[2] /= norm;
        q[3] /= norm;
    }
}

void quaternion_delta(float* q, float* delta, float dt, float* out)
{
    float delta_step[3] = { delta[0] * dt, delta[1] * dt, delta[2] * dt };
    float angular_vel = sqrt(
        delta_step[0] * delta_step[0] +
        delta_step[1] * delta_step[1] +
        delta_step[2] * delta_step[2]
    );
    float theta = angular_vel;

    float q_omega[4];

    if (theta > 1e-6f)
    {
        float theta_half = theta * 0.5f;
        float sin_theta = sinf(theta_half);

        q_omega[0] = cosf(theta_half);
        q_omega[1] = delta_step[0] / angular_vel * sin_theta;
        q_omega[2] = delta_step[1] / angular_vel * sin_theta;
        q_omega[3] = delta_step[2] / angular_vel * sin_theta;
    }
    else
    {
        q_omega[0] = 1.f;
        q_omega[1] = 0.f;
        q_omega[2] = 0.f;
        q_omega[3] = 0.f;
    }

    float tmp[4];
    quaternion_product(q_omega, q, tmp);
    quaternion_normalize(tmp);
    memcpy(out, tmp, sizeof(tmp));
}

void quaternion_diff(float* q1, float* q2, float* out)
{
    float q1_conj[4];
    quaternion_conjugate(q1, q1_conj);
    quaternion_product(q1_conj, q2, out);
}

void quaternion_to_euler(float* q, float* out)
{
    float w = q[0]; float x = q[1]; float y = q[2]; float z = q[3];

    out[0] = atan2f(2.f * (w*x + y*z), 1.f - 2.f * (x*x + y*y));

    float s = 2.f * (w*y - z*x);
    if (s >  1.f) s =  1.f;
    if (s < -1.f) s = -1.f;
    out[1] = asinf(s);

    out[2] = atan2f(2.f * (w*z + x*y), 1.f - 2.f * (y*y + z*z));
}

void quaternion_rotate(float* q, float* v, float* out)
{
    float qx = q[1], qy = q[2], qz = q[3], w = q[0];
    float tx = 2.f * (qy*v[2] - qz*v[1]);
    float ty = 2.f * (qz*v[0] - qx*v[2]);
    float tz = 2.f * (qx*v[1] - qy*v[0]);

    out[0] = v[0] + w*tx + (qy*tz - qz*ty);
    out[1] = v[1] + w*ty + (qz*tx - qx*tz);
    out[2] = v[2] + w*tz + (qx*ty - qy*tx);
}

void quaternion_to_rotvec(float* q, float* out)
{
    float w = q[0]; float x = q[1]; float y = q[2]; float z = q[3];
    if (w < 0.f) { w = -w; x = -x; y = -y; z = -z; }

    float vn = sqrtf(x*x + y*y + z*z);
    float scale;
    if (vn < 1e-6f) scale = 2.f;
    else scale = 2.f * atan2f(vn, w) / vn;

    out[0] = x * scale;
    out[1] = y * scale;
    out[2] = z * scale;
}
