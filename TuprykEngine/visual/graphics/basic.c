#include "basic.h"

#include <math.h>
#include <SDL.h>
#include "window.h"
#include "../../LinAlg/tensor.h"

float global_x_rot = M_PI * 0.25;
float global_y_rot = M_PI * 0.0;
float global_z_rot = 0.0;

void draw_pixel(float x, float y, float r, float g, float b)
{
    SDL_SetRenderDrawColor(renderer, (int) (r * 255.f), (int) (g * 255.f), (int) (b * 255.f), 255);
    SDL_RenderDrawPoint(renderer, x, y);
}

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

void draw_circle(int cx, int cy, int r)
{
    for (int y = -r; y <= r; y++)
    {
        int dx = (int)sqrt(r * r - y * y);
        SDL_RenderDrawLine(
            renderer,
            cx - dx, cy + y,
            cx + dx, cy + y
        );
    }
}

void draw_3d_point(float x, float y, float z)
{
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    struct point_2d p = unit_3d_to_2d(x, y, z);
    draw_circle(p.x * 480.f, p.y * 480.f, 2);
}

void draw_3d_mark(float x, float y, float z)
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
    tensor_free(in);

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

void plot_unit_func_heat(
    float (*func)(struct tensor*),
    float center_x,
    float center_y,
    float area_side
)
{
    int x_shape[1] = {2};
    struct tensor* x = new_tensor(x_shape, 1, NULL);

    int screen_shape[2] = {WINDOW_W, WINDOW_H};
    struct tensor* screen = new_tensor(screen_shape, 2, NULL);

    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            float i_norm = (float) i / (float) WINDOW_W;
            float j_norm = (float) j / (float) WINDOW_W;
            i_norm = (i_norm - 0.5f) * area_side + center_x;
            j_norm = (j_norm - 0.5f) * area_side + center_y;
            x->values[0] = i_norm;
            x->values[1] = j_norm;
            
            float out = func(x);
            int index[] = {i, j};
            screen->values[get_tensor_value_index(screen, index)] = out;
        }
    }

    float max_val = tensor_max(screen);
    float min_val = tensor_min(screen);

    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            int index[] = {i, j};
            float out = screen->values[get_tensor_value_index(screen, index)];
            if (fabsf(out) < 1e-1)
            {
                draw_pixel(i, j, 0, 1.f, 0);
            }
            else
            {
                out = (out - min_val) / (max_val - min_val) * 2.f - 1.f;
                if (out < 0) draw_pixel(i, j, 0, 0, -1.f * out);
                else if (out > 0) draw_pixel(i, j, out, 0, 0);
                else draw_pixel(i, j, 0, 0, 0);
            }
        }
    }
    
    tensor_free(x);
    tensor_free(screen);
}

void plot_unit_func_3d(
    float (*func)(struct tensor*),
    float center_x,
    float center_y,
    float center_z,
    float area_side
)
{
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    int res = 200;
    float step_eval = area_side / res;

    int shape[] = {3};
    struct tensor* in = new_tensor(shape, 1, NULL);

    for (int i = 0; i < res; i++)
    {
        for (int j = 0; j < res; j++)
        {
            for (int k = 0; k < res; k++)
            {
                in->values[0] = ((float)i) * step_eval + center_x - (area_side * 0.5f);
                in->values[1] = ((float)j) * step_eval + center_y - (area_side * 0.5f);
                in->values[2] = ((float)k) * step_eval + center_z - (area_side * 0.5f);

                float v = func(in);

                if (fabsf(v) < 1e-2)
                {
                    draw_3d_point(
                        ((float)i) / ((float)res),
                        ((float)j) / ((float)res),
                        ((float)k) / ((float)res)
                    );
                }
            }
        }
    }
    tensor_free(in);
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
