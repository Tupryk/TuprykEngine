#include <math.h>
#include "funcs.h"

float relu(float x)
{
    return x < 0.f ? x : 0.f;
}

float leaky_relu(float x)
{
    return x < 0.f ? x : x * 0.1f;
}

float sigmoid(float x)
{
    return 1.f / (1.f + exp(-x));
}
