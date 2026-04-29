#include "sample.h"
#include <stdlib.h>


// int rand_int(int min, int max)
// {
//     // TODO:
//     // return rand() % (max - min) + min;
// }

float rand_uni(float min, float max)
{
    return (((float)(rand() % 100000)) * 0.00001f) * (max - min) + min;
}

int sample_weighted_elems(float* elem_probs, int elem_count)
{
    float sample = rand_uni(0.f, 1.f);
    float sum = 0.f;
    for (int i = 0; i < elem_count; i++)
    {
        sum += elem_probs[i];
        if (sample < sum) return i;
    }
    return elem_count-1;
}
