#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../TuprykEngine/ui/audio/audio.h"


int main()
{
    int sample_rate = 44100;
    int duration_sec = 2;
    int num_samples = sample_rate * duration_sec;
    float *samples = malloc(sizeof(float) * num_samples);

    for (int i = 0; i < num_samples; i++) {
        samples[i] = 0.5f * sinf(2.0f * M_PI * 440.0f * i / sample_rate);
    }

    return play_audio(samples, num_samples, sample_rate);
}
