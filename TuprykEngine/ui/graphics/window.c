#include "window.h"
#include <SDL.h>


SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

void init_window()
{
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "TuprykEngine Visualizer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        WINDOW_W, WINDOW_H,
        SDL_WINDOW_SHOWN
    );

    renderer = SDL_CreateRenderer(window, -1, 0);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
}

void window_clear()
{
    SDL_RenderClear(renderer);
}

void window_wait()
{
    SDL_RenderPresent(renderer);

    SDL_Event e;
    while (SDL_WaitEvent(&e)) {
        if (e.type == SDL_QUIT)
            break;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void window_wait_with_func(void (*func)())
{
    SDL_Event e;
    int running = 1;

    while (running)
    {
        while (SDL_PollEvent(&e))
        {
            if (e.type == SDL_QUIT)
                running = 0;
        }

        func();

        SDL_RenderPresent(renderer);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        // SDL_Delay(16); // ~60 FPS
    }
}

int window_wait_time(float dt)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) { if (e.type == SDL_QUIT) return 1; }

    SDL_RenderPresent(renderer);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Delay((int)(dt * 1000.f));

    return 0;
}


void free_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
