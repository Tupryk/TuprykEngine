#include "basic.h"

#include <math.h>
#include <SDL.h>
#include "window.h"
#include "../../LinAlg/tensor.h"

float global_x_rot = M_PI * 0.25;
float global_y_rot = M_PI * 0.0;
float global_z_rot = 0.0;

void draw_line(float start_x, float start_y, float end_x, float end_y)
{
    SDL_RenderDrawLine(renderer, start_x * 480.f, start_y * 480.f, end_x * 480.f, end_y * 480.f);
}

struct point_2d unit_3d_to_2d(float x, float y, float z)
{
    float x0 = x - 0.5f;
    float y0 = y - 0.5f;
    float z0 = z - 0.5f;

    float x1 = x0 * cos(global_z_rot) - y0 * sin(global_z_rot);
    float y1 = x0 * sin(global_z_rot) + y0 * cos(global_z_rot);
    float z1 = z0;

    float y2 = y1 * cos(global_x_rot) - z1 * sin(global_x_rot);
    float z2 = y1 * sin(global_x_rot) + z1 * cos(global_x_rot);
    float x2 = x1;

    float x3 = x2 * cos(global_y_rot) + z2 * sin(global_y_rot);
    float z3 = -x2 * sin(global_y_rot) + z2 * cos(global_y_rot);
    float y3 = y2;

    float depth = z3 + 2.5f;
    if (depth < 0.01f) depth = 0.01f;

    struct point_2d p;
    p.x = x3 / depth + 0.5f;
    p.y = y3 / depth + 0.5f;

    return p;
}

void draw_line_3d(
    float start_x, float start_y, float start_z,
    float end_x, float end_y, float end_z
) {
    struct point_2d s = unit_3d_to_2d(start_x, start_y, start_z);
    struct point_2d e = unit_3d_to_2d(end_x, end_y, end_z);

    draw_line(s.x, s.y, e.x, e.y);
}

void draw_3d_point(float x, float y, float z)
{
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);

    float point_size = 0.05;

    draw_line_3d(x + point_size, y + point_size, z, x - point_size, y - point_size, z);
    draw_line_3d(x + point_size, y - point_size, z, x - point_size, y + point_size, z);
}

void draw_3d_unit_cube()
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    draw_line_3d(0.f, 0.f, 0.f, 1.f, 0.f, 0.f);
    draw_line_3d(1.f, 0.f, 0.f, 1.f, 1.f, 0.f);
    draw_line_3d(1.f, 1.f, 0.f, 0.f, 1.f, 0.f);
    draw_line_3d(0.f, 1.f, 0.f, 0.f, 0.f, 0.f);

    draw_line_3d(0.f, 0.f, 1.f, 1.f, 0.f, 1.f);
    draw_line_3d(1.f, 0.f, 1.f, 1.f, 1.f, 1.f);
    draw_line_3d(1.f, 1.f, 1.f, 0.f, 1.f, 1.f);
    draw_line_3d(0.f, 1.f, 1.f, 0.f, 0.f, 1.f);

    draw_line_3d(0.f, 0.f, 0.f, 0.f, 0.f, 1.f);
    draw_line_3d(0.f, 1.f, 0.f, 0.f, 1.f, 1.f);
    draw_line_3d(1.f, 0.f, 0.f, 1.f, 0.f, 1.f);
    draw_line_3d(1.f, 1.f, 0.f, 1.f, 1.f, 1.f);
}

void plot_unit_func(
    float (*func)(struct tensor*),
    float center_x,
    float center_y,
    float area_side
)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int res = 25;
    float step_eval = area_side / res;
    float step_vis = 1.f / res;

    int shape[] = {2};
    struct tensor* in = new_tensor(shape, 1, NULL);

    float surface[res][res];

    float min =  1e4;
    float max = -1e4;

    for (int i = 0; i < res; i++)
    {
        for (int j = 0; j < res; j++)
        {
            in->values[0] = ((float)i) * step_eval + center_x - (area_side * 0.5f);
            in->values[1] = ((float)j) * step_eval + center_y - (area_side * 0.5f);

            float v = func(in);
            surface[i][j] = v;

            if (max < v) max = v;
            if (min > v) min = v;
        }
    }
    free_tensor(in);

    for (int i = 0; i < res-1; i++)
    {
        for (int j = 0; j < res-1; j++)
        {
            float start_x = ((float)i) * step_vis;
            float start_y = ((float)j) * step_vis;
            float start_z = 1.f - (surface[i][j] - min) / (max - min);

            {
                float end_x = ((float)i+1) * step_vis;
                float end_y = ((float)j) * step_vis;
                float end_z = 1.f - (surface[i+1][j] - min) / (max - min);

                draw_line_3d(start_x, start_z, start_y, end_x, end_z, end_y);
                
                if (i == res-2) {
                    start_x = ((float)i+1) * step_vis;
                    start_y = ((float)j+1) * step_vis;
                    start_z = 1.f - (surface[i+1][j+1] - min) / (max - min);
                    draw_line_3d(start_x, start_z, start_y, end_x, end_z, end_y);
                }
            }

            {
                float end_x = ((float)i) * step_vis;
                float end_y = ((float)j+1) * step_vis;
                float end_z = 1.f - (surface[i][j+1] - min) / (max - min);

                draw_line_3d(start_x, start_z, start_y, end_x, end_z, end_y);
                
                if (j == res-2) {
                    start_x = ((float)i+1) * step_vis;
                    start_y = ((float)j+1) * step_vis;
                    start_z = 1.f - (surface[i+1][j+1] - min) / (max - min);
                    draw_line_3d(start_x, start_z, start_y, end_x, end_z, end_y);
                }
            }
        }
    }
}

void add_global_rot(float delta_x, float delta_y, float delta_z)
{
    global_x_rot += delta_x;
    global_y_rot += delta_y;
    global_z_rot += delta_z;
}

void set_global_rot(float rot_x, float rot_y, float rot_z)
{
    global_x_rot = rot_x;
    global_y_rot = rot_y;
    global_z_rot = rot_z;
}
