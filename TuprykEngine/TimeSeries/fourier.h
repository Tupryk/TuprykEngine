#ifndef FOURIER
#define FOURIER

typedef struct
{
    float frequency;
    float amplitude;
    float phase;
} signal_t;

void dft(float* samples, int sample_size, int sample_rate, int num_frequencies, signal_t* spectrum);
void fft();

#endif
