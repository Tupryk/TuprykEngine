#include <math.h>

#include "utils.h"


void human_made_sequence_hand0(int type, int* out, float* params)
{
    for (int i = 0; i < 7; i++) out[i] = -1;
    params[0] = 100.f; params[1] = 1.f;
    params[2] = 0.f; params[3] = 0.f; params[4] = 0.f;
    params[5] = 0.f;

    switch(type)
    {
        case 0:
        {
            params[2] = 0.2f; params[3] = 0.2f; params[4] = 0.2f;
            out[0] = 1; out[1] = 1; out[2] = 1; out[3] = 1;
            params[5] = 1.f;
            break;
        }
        case 1:
        {
            params[2] = 0.8f; params[3] = 0.2f; params[4] = 0.2f;
            break;
        }
    }
}

void human_made_sequence_hand1(int type, int* out, float* params)
{
    for (int i = 0; i < 7; i++) out[i] = -1;
    params[0] = 100.f; params[1] = 1.f;
    params[2] = 0.f; params[3] = 0.f; params[4] = 0.f;
    params[5] = 0.f;

    switch(type)
    {
        case 0:
        {
            params[2] = 0.2f; params[3] = 0.2f; params[4] = 0.2f;
            out[0] = 1; out[1] = 2; out[2] = 3; out[3] = 4; out[6] = 5;
            break;
        }
        case 1:
        {
            params[2] = 0.2f; params[3] = 0.8f; params[4] = 0.2f;
            out[0] = 1; out[6] = 5;
            break;
        }
        case 2:
        {
            params[2] = 0.2f; params[3] = 0.2f; params[4] = 0.8f;
            out[1] = 2; out[6] = 5;
            break;
        }
        case 3:
        {
            params[2] = 0.8f; params[3] = 0.8f; params[4] = 0.2f;
            out[2] = 3; out[6] = 5;
            break;
        }
        case 4:
        {
            params[2] = 0.2f; params[3] = 0.8f; params[4] = 0.8f;
            out[3] = 4; out[6] = 5;
            break;
        }
        case 5:
        {
            params[2] = 0.8f; params[3] = 0.2f; params[4] = 0.2f;
            break;
        }
    }
}

void human_made_implicit_hand(float* pos, float* out)
{
    if (
        (
            pos[2] < -0.4 &&
            !(pos[0] > 0.1 && pos[1] > 0.1) &&
            !(pos[0] < -0.1 && pos[1] > 0.1) &&
            !(pos[0] > 0.1 && pos[1] < -0.1) &&
            !(pos[0] < -0.1 && pos[1] < -0.1)
        ) || (
            0
            // pos[2] < -0.3 &&
            // (
            //     (pos[0] >  0.3 && pos[1] < 0.1 && pos[1] > -0.1) ||
            //     (pos[1] >  0.3 && pos[0] < 0.1 && pos[0] > -0.1) ||
            //     (pos[0] < -0.3 && pos[1] < 0.1 && pos[1] > -0.1) ||
            //     (pos[1] < -0.3 && pos[0] < 0.1 && pos[0] > -0.1)
            // )
        )
    )
    out[0] = 1.f;
    else out[0] = 0.f;
    out[6] = 0.f;

    out[1] = 2000.f;

    if (pos[2] < -0.475)
    {
        out[3] = 0.f;
        out[4] = 0.f;
        out[5] = 1.f;
        
        out[2] = -0.65f;
    }

    else if (pos[2] < -0.425)
    {
        out[3] = 0.f;
        out[4] = 1.f;
        out[5] = 0.f;
        
        out[2] = 0.f;
    }

    else if (pos[2] < -0.4)
    {
        out[3] = 1.f;
        out[4] = 0.f;
        out[5] = 0.f;

        out[2] = 0.f;

        // Fixed
        if (
            pos[0] < 0.1 && pos[1] < 0.1 &&
            pos[0] > -0.1 && pos[1] > -0.1
        ) {
            out[3] = 0.f;
            out[4] = 0.f;
            out[5] = 0.f;
            out[6] = 1.f;
        }
    }

    else if (pos[2] < -0.29)
    {
        out[3] = 1.f;
        out[4] = 0.f;
        out[5] = 1.f;
        
        out[2] = 0.f;
    }
}

void human_made_implicit_cube(float* pos, float* out)
{
    out[0] = 1.f;     // Exists
    out[1] = 1000.f;  // Stiffness
    out[2] = 0.f;     // CTE
    out[3] = 1.f;     // R
    out[4] = 1.f;     // G
    out[5] = 0.f;     // B
    out[6] = 0.f;     // Fixed
}

void fixed_grasp_motion(struct VoxelSim* vs, float start_time, float end_time, float z_depth)
{
    vs->temp = 0.f;
    float z_vel = 0.f;
    float current_time = ((float)vs->t) * vs->tau;
    float total_time = end_time - start_time;
    float norm_time = (current_time - start_time) / total_time;

    float phase_2_start = 0.3f;
    float phase_3_start = 0.6f;

    if (norm_time <= 1.f)
    {
        if (norm_time < phase_2_start)  // Phase I move down
        {
            z_vel = z_depth / (phase_2_start * total_time);
        }
        else if (norm_time > phase_2_start && norm_time < phase_3_start)  // Phase II close gripper
        {
            float t = (norm_time - phase_2_start) / (phase_3_start - phase_2_start);
            vs->temp = sinf(t * M_PI * 0.5f);
        }
        else  // Phase III move up
        {
            vs->temp = 1.f;
            z_vel = -1.f * (z_depth / ((1.f - phase_3_start) * total_time));
        }
    }
    else
    {
        vs->temp = 1.f;
    }

    for (int i = 0; i < vs->count; i++)
    {
        if (vs->fixed[i])
        {
            vs->vel->values[i*3 + 2] = z_vel;
        }
    }
}
