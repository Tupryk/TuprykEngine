#define DEBUG

#include <SDL.h>
#include "../../TuprykEngine/LinAlg/tensor.h"
#include "../../TuprykEngine/visual/graphics/window.h"
#include "../../TuprykEngine/visual/graphics/basic.h"

struct Triangle
{
    float a_x;
    float a_y;

    float b_x;
    float b_y;

    float c_x;
    float c_y;
};

void draw_triangle(struct Triangle* t)
{
    draw_line(t->a_x, t->a_y, t->b_x, t->b_y);
    draw_line(t->b_x, t->b_y, t->c_x, t->c_y);
    draw_line(t->c_x, t->c_y, t->a_x, t->a_y);
}

float lorenz_product(struct tensor* a, struct tensor* b)
{
    float out = 0.f;
    int n = a->shape[0]-1;
    for (int i = 0; i < n; i++)
    {
        out += a->values[i] * b->values[i];
    }
    out -= a->values[n] * b->values[n];
    return out;
}

float scalar_product(struct tensor* a, struct tensor* b)
{
    float out = 0.f;
    for (int i = 0; i < a->shape[0]; i++)
    {
        out += a->values[i] * b->values[i];
    }
    return out;
}

float eucledian_case(struct tensor* x) { return scalar_product(x, x) - 1.f; }
float hyperbolic_case1(struct tensor* x) { return lorenz_product(x, x) + 1.f; }
float hyperbolic_case2(struct tensor* x) { return lorenz_product(x, x) - 1.f; }
float hyperbolic_case3(struct tensor* x) { return lorenz_product(x, x); }

void rotating_plot1()
{
    plot_unit_func_3d(hyperbolic_case1, 0.f, 0.f, 0.f, 10.f);
    draw_3d_unit_cube();

    add_global_rot(0.0f, 0.01f, 0.0f);
}

int main()
{
    init_window();
    
    // struct Triangle t0;
    // t0.a_x = 0.1f;
    // t0.a_y = 0.5f;

    // t0.b_x = 0.2f;
    // t0.b_y = 0.35f;

    // t0.c_x = 0.3f;
    // t0.c_y = 0.6f;

    // draw_triangle(&t0);

    //-- Hyperbolic space --//
    plot_unit_func_heat(eucledian_case, 0.f, 0.f, 4.f);
    window_wait();
    plot_unit_func_heat(hyperbolic_case1, 0.f, 0.f, 4.f);
    window_wait();
    plot_unit_func_heat(hyperbolic_case2, 0.f, 0.f, 4.f);
    window_wait();
    plot_unit_func_heat(hyperbolic_case3, 0.f, 0.f, 4.f);
    window_wait();

    // TODO: "Set" function which return a bool

    // plot_unit_func(hyperbolic_case1, 0.f, 0.f, 10.f);
    // window_wait();
    // plot_unit_func(hyperbolic_case2, 0.f, 0.f, 10.f);
    // window_wait();
    // plot_unit_func(hyperbolic_case3, 0.f, 0.f, 10.f);
    // window_wait();

    window_wait_with_func(rotating_plot1);

    free_window();
    return 0;
}
