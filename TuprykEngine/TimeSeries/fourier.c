#include <math.h>
#include "fourier.h"


// Discrete Fourier Transform
// Complexity: O(N^2)
void dft(float* samples, int sample_size, int sample_rate, int num_frequencies, signal_t* spectrum)
{
    float freq_step = (float) sample_rate / (float) sample_size;
    
    for (int i = 0; i < num_frequencies; i++)
    {
        float com_x = 0.f;
        float com_y = 0.f;
        for (int j = 0; j < sample_size; j++)
        {
            float angle = (float) j / (float) sample_size * 2.f * M_PI * (float) i;
            float real =  cos(angle);
            float im   = -sin(angle);
            com_x += real * samples[j];
            com_y += im   * samples[j];
        }
        com_x /= (float) sample_size;
        com_y /= (float) sample_size;

        float scale = (i == 0 || (sample_size % 2 == 0 && i == sample_size/2)) ? 1.f : 2.f;
        spectrum[i].amplitude = sqrt(com_x * com_x + com_y * com_y) * scale;
        spectrum[i].frequency = (float) i * freq_step;
        spectrum[i].phase     = -atan2(com_y, com_x);
    }
}

void fft()
{

}
