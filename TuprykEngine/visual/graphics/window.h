#ifndef GRAPHICS_WINDOW
#define GRAPHICS_WINDOW

#include <SDL.h>

#define WINDOW_W 480
#define WINDOW_H 480

SDL_Window* window;
SDL_Renderer* renderer;

void init_window();
void window_wait();
void window_wait_with_func(void (*func)());
void free_window();

#endif
