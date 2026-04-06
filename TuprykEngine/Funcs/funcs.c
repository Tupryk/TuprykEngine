#include "funcs.h"

float relu(float x)
{
    return x < 0.f ? x : 0.f;
}
