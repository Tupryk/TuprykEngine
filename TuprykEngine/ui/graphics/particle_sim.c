#include <stdio.h>

#include "particle_sim.h"
#include "../../../TuprykEngine/ui/graphics/window.h"
#include "../../../TuprykEngine/ui/graphics/basic.h"


void render_ps(struct ParticleSim* ps, tensor_t* cam_pos)
{
    float max_dist = 2000.f;

    for (int i = 0; i < ps->max_count; i++)
    {
        if (ps->energy->values[i] <= 0.f) continue;
        
        float px = ps->pos->values[i * 3];
        float py = ps->pos->values[i * 3 + 1];
        float pz = ps->pos->values[i * 3 + 2];

        float cam_dist_vec[] = {
            px - cam_pos->values[0],
            py - cam_pos->values[1],
            pz - cam_pos->values[2]
        };
        float cam_dist = sqrtf(
            cam_dist_vec[0] * cam_dist_vec[0] +
            cam_dist_vec[1] * cam_dist_vec[1] +
            cam_dist_vec[2] * cam_dist_vec[2]
        );

        if (cam_dist_vec[1] > 0.f && cam_dist > 0.01f && cam_dist < max_dist)
        {
            float intensity = 1.f - cam_dist / max_dist;
            set_rgba(
                ps->color->values[i*3] * intensity,
                ps->color->values[i*3+1] * intensity,
                ps->color->values[i*3+2] * intensity,
                0.5f
            );

            if (
                px / cam_dist_vec[1] < 1.1f && px / cam_dist_vec[1] > -1.1f &&
                pz / cam_dist_vec[1] < 1.1f && pz / cam_dist_vec[1] > -1.1f
            )
            {
                draw_circle(
                    px / cam_dist_vec[1] * WINDOW_W + WINDOW_W * .5f,
                    WINDOW_H - (pz / cam_dist_vec[1] * WINDOW_H + WINDOW_H * .5f),
                    ps->sizes->values[i] / cam_dist_vec[1] * WINDOW_W
                );
            }
        }
    }
}
