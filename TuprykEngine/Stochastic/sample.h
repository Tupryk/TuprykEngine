#ifndef SAMPLE
#define SAMPLE

// int rand_int(int min, int max);
float rand_uni(float min, float max);
float rand_gauss(float mean, float std);
int sample_weighted_elems(float* elem_probs, int elem_count);

#endif
