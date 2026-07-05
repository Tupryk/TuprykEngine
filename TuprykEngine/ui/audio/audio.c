#include "audio.h"

#include <SDL.h>
#include <stdio.h>
#include <stdlib.h>


int play_audio(float* samples, int num_samples, int sample_rate)
{
     if (SDL_Init(SDL_INIT_AUDIO) < 0) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_AudioSpec want, have;
    SDL_zero(want);
    want.freq = sample_rate;
    want.format = AUDIO_F32SYS;
    want.channels = 1;
    want.samples = 4096;
    want.callback = NULL;

    SDL_AudioDeviceID dev = SDL_OpenAudioDevice(NULL, 0, &want, &have, 0);
    if (dev == 0) {
        printf("Failed to open audio device: %s\n", SDL_GetError());
        return 1;
    }

    SDL_QueueAudio(dev, samples, num_samples * sizeof(float));
    SDL_PauseAudioDevice(dev, 0);

    while (SDL_GetQueuedAudioSize(dev) > 0) {
        SDL_Delay(100);
    }

    SDL_CloseAudioDevice(dev);
    SDL_Quit();
    return 0;
}
