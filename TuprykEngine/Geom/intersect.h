#ifndef GEOM_INTERSECT
#define GEOM_INTERSECT

#include "../LinAlg/tensor.h"

struct typedef
{
    int intersection_count;
    tensor* inter1;
    tensor* inter2;
} bl_inter_data;

bl_inter_data ball_line_intersect(tensor* line_point, tensor* line_dir, tensor* ball_point, float radius);

#endif
