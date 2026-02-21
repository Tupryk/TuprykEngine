#include "pdfs.h"
#include "math.h"


float exponential_pdf(float x, float lambda)
{
    return x >= 0 ? lambda * exp(-1.f * lambda * x) : 0.f;
}
