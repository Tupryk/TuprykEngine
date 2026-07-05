#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../TuprykEngine/ui/audio/audio.h"
#include "../../TuprykEngine/TimeSeries/fourier.h"


int main()
{
    int sample_rate = 44100;
    int duration_sec = 2;
    int num_samples = sample_rate * duration_sec;
    float samples[num_samples];

    for (int i = 0; i < num_samples; i++) {
        samples[i] = 0.5f * sinf(2.0f * M_PI * 440.0f * i / sample_rate);
    }

    int num_frequencies = 1000;
    signal_t spectrum[num_frequencies];
    dft(samples, num_samples, sample_rate, num_frequencies, spectrum);

    float r_samples[num_samples];
    memset(r_samples, 0, sizeof(r_samples));
    for (int i = 0; i < num_frequencies; i++)
    {
        printf("----------- Frequency %d -----------\n", i);
        printf("Amplitude: %f\n", spectrum[i].amplitude);
        printf("Frequency: %f\n", spectrum[i].frequency);
        printf("Phase: %f\n", spectrum[i].phase);

        for (int j = 0; j < num_samples; j++)
        {
            r_samples[j] += spectrum[i].amplitude * sinf(2.0f * M_PI * spectrum[i].frequency * j / sample_rate + spectrum[i].phase);
        }
    }

    printf("Playing Original Audio...\n");
    play_audio(samples, num_samples, sample_rate);
    printf("Playing Reconstructed Audio...\n");
    play_audio(r_samples, num_samples, sample_rate);

    return 0;
}
