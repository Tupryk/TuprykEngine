#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "masem.h"

#include "nhr.h"
#include "../augmented_lagrangian.h"
#include "../../../Stochastic/sample.h"
#include "../../../Algos/spatial_data.h"

#include "../../../ui/prints/linalg.h"
#include "../../../ui/graphics/basic.h"
#include "../../../ui/graphics/window.h"


void draw_x_(nlp_t* nlp, tensor_t* x)
{
    int feasible = nlp_feasible(nlp, x);

    if (feasible) set_color(0.1216f, 0.4667f, 0.7059f);
    else set_color(1.0f, 0.4980f, 0.0549f);

    int cx = ((x->values[0] + 1.f) * 0.5f) * ((float) WINDOW_W);
    int cy = ((-x->values[1] + 1.f) * 0.5f) * ((float) WINDOW_H);
    draw_circle(cx, cy, 2);
}

void masem_sample(nlp_t* nlp, int sample_count, float init_scale, float temperature, int iterations, int k, tensor_t** output)
{
    printf("Projecting into feasible space...\n");
    int rejuvenation_steps = 1;
    tensor_t* nhr_samples[rejuvenation_steps];
    aug_lagrangian_init(nlp, 1e-2, 1e-2, 10, 10, 1);
    for (int i = 0; i < sample_count; i++)
    {
    printf("1...\n");
        // Initial sample
        output[i] = new_tensor_vector(2, NULL);
        tensor_fill_gauss(output[i], 0, init_scale);
        
    printf("2...\n");
        // Move to the feasible set
        struct nlp_optim_logs* col = aug_lagrangian_run(output[i]);
        tensor_transfer_values(output[i], col->final_x);
        
    printf("3...\n");
        // Rejuvenate
        nhr_sample(nlp, output[i], rejuvenation_steps, 2.f, nhr_samples);
        tensor_transfer_values(output[i], nhr_samples[rejuvenation_steps-1]);
        for (int j = 0; j < rejuvenation_steps; j++) tensor_free(nhr_samples[j]);
    printf("4...\n");
    }
    aug_lagrangian_free();

    ////////////////////////////////////
    init_window();
    set_color(1.f, 1.f, 1.f);
    window_clear();
    
    for (int i = 0; i < sample_count; i++)
    {
        draw_x_(nlp, output[i]);
        tensor_free(output[i]);
    }
    window_wait();
    free_window();
    ////////////////////////////////////

    printf("Importance resampling...\n");
    float weights[sample_count];
    int indices[k+1];
    float dists[k+1];
    tensor_t* query = tensor_copy_shape(output[0]);
    tensor_t* resampled_points[sample_count];

    for (int i = 0; i < iterations; i++)
    {
        // Calculate resampling weights
        kd_tree_t* kt = kd_tree_init(output, sample_count);
        
        float w_sum = 0.f;
        for (int j = 0; j < sample_count; j++)
        {
            tensor_transfer_values(query, output[j]);

            kd_tree_knn(kt, query, k, indices, dists);
            
            float w_bar = powf(dists[k], temperature); // Ignore itself
            weights[j] = w_bar;
            w_sum += w_bar;
        }
        for (int j = 0; j < sample_count; j++) weights[j] /= w_sum;

        // Resample points based on weights
        for (int j = 0; j < sample_count; j++)
        {
            int idx = sample_weighted_elems(weights, sample_count);
            resampled_points[j] = tensor_copy(output[idx]);
        }
        // Rejuvenate
        for (int j = 0; j < sample_count; j++)
        {
            tensor_free(output[j]); output[j] = resampled_points[j];

            nhr_sample(nlp, output[i], rejuvenation_steps, 2.f, nhr_samples);
            tensor_transfer_values(output[i], nhr_samples[rejuvenation_steps-1]);
            for (int k_ = 0; k_ < rejuvenation_steps; k_++) tensor_free(nhr_samples[k_]);
        }

        kd_tree_free(kt);
    }
    
    tensor_free(query);
}
