#ifndef GEOM_INTERSECT
#define GEOM_INTERSECT

#include "../LinAlg/tensor.h"

struct typedef
{
    int intersection_count;
    tensor_t* inter1;
    tensor_t* inter2;
} bl_inter_data;

bl_inter_data ball_line_intersect(tensor_t* line_point, tensor_t* line_dir, tensor_t* ball_point, float radius);

#endif
