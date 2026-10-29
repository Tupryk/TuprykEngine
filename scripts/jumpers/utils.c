#include <math.h>
#include <stdio.h>

#include "utils.h"

void human_made_cellular_automaton(float* in, float* out)
{
    int feature_dim = 4;
    for (int i = 0; i < feature_dim; i++) out[i] = 0.f;
    
    // if (in[feature_dim*3] > 0.1f)
    {
        out[0] = 1.f;
        out[2] = 1.f;
    }
    if (in[0] > 0.1f)
    {
        out[0] = 1.f;
        out[1] = 1.f;
    }
}
