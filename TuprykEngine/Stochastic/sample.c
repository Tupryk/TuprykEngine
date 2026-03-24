#include "sample.h"
#include <stdlib.h>


float rand_uni(float min, float max)
{
    return (((float)(rand() % 100000)) * 0.00001) * (max - min) + min;
}
