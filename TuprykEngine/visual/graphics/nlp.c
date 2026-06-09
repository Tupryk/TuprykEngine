#include "nlp.h"
#include "basic.h"

#include <math.h>
#include <SDL.h>
#include "window.h"
#include "../../Algos/lists.h"
#include "../../LinAlg/tensor.h"


void draw_nlp(nlp_t* nlp, float square_side)
{
    tensor* eval_point = new_tensor_vector(nlp->dim, NULL);
    float max_cost = 0.f;
    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            eval_point->values[0] = ((float) i) / ((float) WINDOW_W) * square_side*2 - square_side;
            eval_point->values[1] = ((float) j) / ((float) WINDOW_H) * square_side*2 - square_side;

            if (!nlp_feasible(nlp, eval_point))
            {
                float c = nlp_infeasible_cost(nlp, eval_point);
                if (c > max_cost) max_cost = c;
            }
        }
    }
    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            eval_point->values[0] = ((float) i) / ((float) WINDOW_W) * square_side*2 - square_side;
            eval_point->values[1] = ((float) j) / ((float) WINDOW_H) * square_side*2 - square_side;

            if (nlp_feasible(nlp, eval_point)) draw_pixel(i, j, 0.f, .8f, 0.f);
            else 
            {
                float c = nlp_infeasible_cost(nlp, eval_point) / max_cost;
                draw_pixel(i, j, c, 0.f, 1.f - c);
            }
        }
    }
    tensor_free(eval_point);
}

void draw_nlp_foam(nlp_t* nlp, foam_t* foam, float square_side, int mark_gaussians)
{
    tensor* eval_point = new_tensor_vector(nlp->dim, NULL);
    float max_cost = 0.f;
    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            eval_point->values[0] = ((float) i) / ((float) WINDOW_W) * square_side*2 - square_side;
            eval_point->values[1] = ((float) j) / ((float) WINDOW_H) * square_side*2 - square_side;

            if (!nlp_feasible(nlp, eval_point))
            {
                float c = nlp_sos_const_eval(nlp, eval_point);
                if (!mark_gaussians) c += foam_eval(foam, nlp, eval_point);
                if (c > max_cost) max_cost = c;
            }
        }
    }
    for (int i = 0; i < WINDOW_W; i++)
    {
        for (int j = 0; j < WINDOW_H; j++)
        {
            eval_point->values[0] = ((float) i) / ((float) WINDOW_W) * square_side*2 - square_side;
            eval_point->values[1] = ((float) j) / ((float) WINDOW_H) * square_side*2 - square_side;

            if (nlp_feasible(nlp, eval_point)) draw_pixel(i, j, 0.f, .8f, 0.f);
            else 
            {
                float c = nlp_sos_const_eval(nlp, eval_point);
                if (!mark_gaussians) c += foam_eval(foam, nlp, eval_point);
                c /= max_cost;
                draw_pixel(i, j, c, 0.f, 1.f - c);
            }
        }
    }
    tensor_free(eval_point);
    if (mark_gaussians)
    {
        struct stack_elem* se = foam->force_fields->next;
        while (se != NULL)
        {
            gaussian_t* gauss = (gaussian_t*) se->data;
            
            int cx = ((gauss->mu->values[0] + square_side) / (square_side*2)) * ((float) WINDOW_W);
            int cy = ((-gauss->mu->values[1] + square_side) / (square_side*2)) * ((float) WINDOW_H);
            set_color(1.f, 0.f, 1.f);
            draw_circle(cx, cy, WINDOW_W * foam->inital_std / (square_side*2));

            // draw_gaussian(ball_center, A, WINDOW_W * foam_radius / (square_side*2));
            se = se->next;
        }
    }
}
