#include "utils.h"

float clip(float x, float min, float max)
{
    if (x < min) return min;
    if (x > max) return max;
    return x;
}

float min(float a, float b) { return a < b ? a : b; }
float max(float a, float b) { return a > b ? a : b; }

float rescale(float x, float x_min, float x_max, float y_min, float y_max)
{
    return ((x - x_min) / (x_max - x_min)) * (y_max - y_min) + y_min;
}
