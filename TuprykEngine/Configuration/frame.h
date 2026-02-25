#ifndef CONFIG_FRAME
#define CONFIG_FRAME

#include "../LinAlg/tensor.h"


struct frame
{
    struct tensor* pos;
    struct tensor* rot;
    
    struct frame* parent;

    // 0: None, 1: Triangulated, 2: Implicit
    // There should be different mesh types for collisions and visuals
    int mesh_type;
};

void frame_free(struct frame* f);

#endif
