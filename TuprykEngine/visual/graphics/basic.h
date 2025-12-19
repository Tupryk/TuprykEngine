#ifndef GRAPHICS_BASIC
#define GRAPHICS_BASIC

#include <math.h>
#include "../../LinAlg/tensor.h"

struct point_2d
{
    float x;
    float y;
};

void draw_line(float start_x, float start_y, float end_x, float end_y);
void draw_line_3d(
    float start_x, float start_y, float start_z,
    float end_x, float end_y, float end_z
);
void draw_3d_point(float x, float y, float z);
void draw_3d_unit_cube();
void plot_unit_func(
    float (*func)(struct tensor*),
    float center_x,
    float center_y,
    float area_side
);
void add_global_rot(float delta_x, float delta_y, float delta_z);
void set_global_rot(float rot_x, float rot_y, float rot_z);

#endif
