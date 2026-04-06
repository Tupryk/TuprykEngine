#include "sample.h"
#include <stdlib.h>


// int rand_int(int min, int max)
// {
//     // TODO:
//     // return rand() % (max - min) + min;
// }

float rand_uni(float min, float max)
{
    return (((float)(rand() % 100000)) * 0.00001) * (max - min) + min;
}
