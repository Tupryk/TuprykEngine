#ifndef NHR
#define NHR

#include "../nlp.h"
#include "../../../LinAlg/tensor.h"

void nhr_sample(nlp_t* nlp, tensor* feasible_point, int sample_count, float delta_max, tensor** output);

#endif
