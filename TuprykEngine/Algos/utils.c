#include "utils.h"

float clip(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }
