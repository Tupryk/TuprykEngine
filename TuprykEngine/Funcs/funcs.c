#include <math.h>
#include "funcs.h"

float linear(float x) { return x; }
float relu(float x) { return x < 0.f ? x : 0.f; }
float leaky_relu(float x) { return x < 0.f ? x : x * 0.1f; }
float sigmoid(float x) { return 1.f / (1.f + exp(-x)); }
float gaussian(float x) { return exp(-x * x); }  // Does not integrate to 1!
