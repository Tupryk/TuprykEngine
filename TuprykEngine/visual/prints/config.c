#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "linalg.h"
#include "../../Algos/lists.h"


void print_spaces(int count)
{
    for (int i = 0; i < count; i++)
    {
        printf("  ");
    }
}

void print_frame(config* C, frame* f, int depth)
{
    if (depth >= 3)
    {
        print_spaces(depth);
        printf("(...)\n");
        return;
    }
    print_spaces(depth);
    printf("Frame (depth %d):\n", depth);
    float* pos = f->pos->values;
    float* rot = f->rot->values;
    
    print_spaces(depth);
    printf("-> Position: [%g, %g, %g]\n", pos[0], pos[1], pos[2]);
    print_spaces(depth);
    printf("-> Rotation: [%g, %g, %g, %g]\n", rot[0], rot[1], rot[2], rot[3]);
    print_spaces(depth);
    printf("-> Type: ");
    switch(f->type)
    {
        case 0:
            printf("Marker\n");
            break;
        case 1:
            printf("Ball\n");
            float r = *(float*) f->data;
            print_spaces(depth);
            printf("  -> Radius: %g\n", r);
            break;
        case 2:
            printf("Camera\n");
            break;
        case 3:
            printf("Light\n");
            break;
        default:
            printf("Unknown\n");
            break;
    }

    print_spaces(depth);
    printf("-> Children: [");
    for (int i = 0; i < f->children_count; i++)
    {
        printf("%d, ", f->children[i]);
    }
    printf("]\n");

    for (int i = 0; i < f->children_count; i++)
    {
        print_frame(C, C->frames[f->children[i]], depth+1);
    }
}

void print_config(config* C)
{
    printf("Total frame count: %d\n", C->frame_count);
    if (C->frame_count)
    {
        print_frame(C, C->frames[0], 0);
    }
}
