#define DEBUG

#include <SDL.h>
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/visual/graphics/window.h"
#include "../../../TuprykEngine/visual/graphics/basic.h"


struct Quadratic
{
    struct tensor* H;
    struct tensor* v;
    float c;
};

struct Quadratic q;

void quadratic_init()
{
    int shape_H[] = {2, 2};
    float values_H[] = {1.f, 0.f, 0.f, 1.f};
    q.H = new_tensor(shape_H, 2, values_H);

    int shape_v[] = {2};
    float values_v[] = {0.f, 0.f};
    q.v = new_tensor(shape_v, 1, values_v);
    tensor_transpose(q.v);

    q.c = 0.f;
}

void quadratic_free()
{
    free_tensor(q.H);
    free_tensor(q.v);
}

float quadratic_eval(struct tensor* x)
{
    struct tensor* x_T = tensor_copy(x);
    tensor_transpose(x_T);
    
    struct tensor* dot0 = tensor_mult_give(q.H, x_T);
    struct tensor* dot = tensor_mult_give(x, dot0);
    struct tensor* a = tensor_mult_give(x, q.v);
    
    float out = dot->values[0] + a->values[0] + q.c;
    
    free_tensor(x_T);
    free_tensor(dot0);
    free_tensor(dot);
    free_tensor(a);
    
    return out;
}

void rotating_plot()
{
    plot_unit_func(quadratic_eval, 0.f, 0.f, 20.f);
    draw_3d_point(0.5f, 0.5f, 0.5f);
    draw_3d_unit_cube();

    add_global_rot(0.0f, 0.01f, 0.0f);
}

int main()
{
    init_window();
    
    for (float i = 0.f; i < 1.f; i+=0.025f) {
            draw_line(0, 0, i, 1.f);
        }
        
    for (float i = 0.f; i < 1.f; i+=0.025f) {
        draw_line(0, 0, 1.f, i);
    }

    window_wait();

    draw_3d_unit_cube();

    window_wait();

    // Plot a function
    quadratic_init();
    
    plot_unit_func(quadratic_eval, 0.f, 0.f, 20.f);
    window_wait();

    window_wait_with_func(rotating_plot);
    
    quadratic_free();

    free_window();
    return 0;
}
