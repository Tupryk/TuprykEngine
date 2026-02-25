#include "frame.h"


void frame_free(struct frame* f)
{
    tensor_free(f->pos);
    tensor_free(f->rot);
    free(f);
}
