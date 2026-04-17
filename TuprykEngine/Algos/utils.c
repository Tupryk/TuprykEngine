#include "utils.h"

float clip(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
