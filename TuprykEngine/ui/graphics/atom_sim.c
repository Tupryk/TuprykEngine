#include <stdio.h>

#include "atom_sim.h"
#include "../../../TuprykEngine/ui/graphics/ogl.h"


void render_ps(struct AtomSim* ps)
{
    float* features = ps->features->values;
    int feature_dim = ps->features->shape[1];

    for (int i = 0; i < ps->count; i++)
    {
        int i3 = i * 3;
        
        float px = ps->pos->values[i3];
        float py = ps->pos->values[i3 + 1];
        float pz = ps->pos->values[i3 + 2];

        float r = (features[i * feature_dim    ] + 1.f) * 0.5f;
        float g = (features[i * feature_dim + 1] + 1.f) * 0.5f;
        float b = (features[i * feature_dim + 2] + 1.f) * 0.5f;

        ogl_draw_sphere(px, py, pz, 1.f, r, g, b);
    }

    // // Draw links
    // struct stack_elem* current_elem = ps->link_data->next;
    // while (current_elem != NULL)
    // {
    //     link_t* link = (link_t*) current_elem->data;
    //     current_elem = current_elem->next;

    //     set_rgba(1.f, 1.f, 1.f, 0.7f);
    //     draw_line_px(
    //         points_2d[link->from][0], points_2d[link->from][1],
    //         points_2d[link->to][0], points_2d[link->to][1]
    //     );
    // }
}
