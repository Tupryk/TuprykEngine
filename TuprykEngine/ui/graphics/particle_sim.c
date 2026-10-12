#include <stdio.h>

#include "particle_sim.h"
#include "../../../TuprykEngine/ui/graphics/ogl.h"


void render_ps(struct ParticleSim* ps)
{
    float aa = ps->max_age + ps->max_food_age;

    /* Draw particles */
    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->age->values[i] >= aa) continue; /* skip dead particles */
        int i3 = i * 3;

        float px = ps->pos->values[i3];
        float py = ps->pos->values[i3 + 1];
        float pz = ps->pos->values[i3 + 2];

        float r = ps->color->values[i3];
        float g = ps->color->values[i3 + 1];
        float b = ps->color->values[i3 + 2];

        float radius = ps->sizes->values[i];

        if (ps->age->values[i] < ps->max_age)
        {
            ogl_draw_sphere(px, py, pz, radius, r, g, b);
        }
        else
        {
            ogl_draw_sphere(px, py, pz, radius, 0.f, 0.8f, 0.f);
        }
    }

    /* Draw links as semi-transparent white lines between particles */
    struct stack_elem* current_elem = ps->link_data->next;
    while (current_elem != NULL)
    {
        link_t* link = (link_t*) current_elem->data;
        current_elem = current_elem->next;

        int from = link->from;
        int to = link->to;

        int f3 = from * 3;
        int t3 = to * 3;

        ogl_draw_line(ps->pos->values[f3], ps->pos->values[f3 + 1], ps->pos->values[f3 + 2],
                      ps->pos->values[t3], ps->pos->values[t3 + 1], ps->pos->values[t3 + 2],
                      1.f, 1.f, 1.f, 0.7f);
    }
}