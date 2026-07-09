#define DEBUG

#include <stdio.h>
#include "../../../TuprykEngine/Control/cart.h"
#include "../../../TuprykEngine/LinAlg/tensor.h"
#include "../../../TuprykEngine/ui/graphics/basic.h"
#include "../../../TuprykEngine/ui/graphics/window.h"

int main()
{
    float dt = 0.01f;

    struct cart_config C;
    C.c_mass = 1.0f;
    C.p_mass = 0.2f;
    C.p_length_to_com = 0.5f;
    C.g = 9.81f;

    struct cart_state state;
    state.x = 0.f;
    state.x_dot = 0.f;
    state.theta = 0.174532f;
    state.theta_dot = 0.f;

    printf("Cart Config:\n");
    printf("- c_mass: %f\n", C.c_mass);
    printf("- p_mass: %f\n", C.p_mass);
    printf("- p_length_to_com: %f\n", C.p_length_to_com);
    printf("- g: %f\n", C.g);

    printf("Initial State:\n");
    printf("- x: %f\n", state.x);
    printf("- x_dot: %f\n", state.x_dot);
    printf("- theta: %f\n", state.theta);
    printf("- theta_dot: %f\n", state.theta_dot);

    init_window();

    float time = 0.f;
    for (int i = 0; i < 10000; i++)
    {
        time += dt;
        cart_step(&C, &state, dt);

        float l = 0.25f;
        float y = 0.25f;
        float vc = state.x + 0.5f;
        set_color(1.f, 1.f, 1.f);
        draw_line(vc - 0.1f, y, vc + 0.1f, y);  // Cart
        draw_line(vc, y, sinf(state.theta) * l + vc, - cosf(state.theta) * l + y);  // Pole
        
        printf("%f: %f, %f, %f, %f\n", time, state.x, state.x_dot, state.theta, state.theta_dot);

        if(window_wait_time(dt)) break;
    }

    free_window();

    return 0;
}
