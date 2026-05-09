#ifndef MASEM
#define MASEM

#include "../nlp.h"
#include "../../../LinAlg/tensor.h"

void masem_sample(nlp_t* nlp, int sample_count, float init_scale, float temperature, int iterations, int k, tensor** output);

#endif
