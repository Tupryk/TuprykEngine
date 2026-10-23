#include <stdio.h>

#include "voxels_vis.h"


void render_voxels(struct VoxelSim* vs)
{
    int* links = vs->links;
    float* pos = vs->pos->values;
    float* color = vs->color->values;
    float radius = vs->voxel_dim * 0.5f;

    draw_floor(vs->floor_z, 0.8f, 0.8f, 0.8f, 0.8f);

    for (int i = 0; i < vs->count; i++)
    {
        int i3 = i*3;
        ogl_draw_sphere(
            pos[i3], pos[i3+1], pos[i3+2],
            radius,
            color[i3], color[i3+1], color[i3+2]
        );

        // Draw links
        for (int j = 1; j < 6; j+=2)
        {
            float c[3] = { 0.f, 0.f, 0.f };
            c[j%3] = 1.f;
            int j3 = links[i*6 + j];
            if (j3 == -1) continue;
            j3 *= 3;
            ogl_draw_line(
                pos[i3], pos[i3+1], pos[i3+2],
                pos[j3], pos[j3+1], pos[j3+2],
                c[0], c[1], c[2], 1.f
            );
        }
    }
}
