#ifndef GRAPHICS_WINDOW
#define GRAPHICS_WINDOW

#include <SDL.h>

#define WINDOW_W 640
#define WINDOW_H 640

extern SDL_Window* window;
extern SDL_Renderer* renderer;

void init_window();
void window_clear();
void window_wait();
void window_wait_with_func(void (*func)());
int window_wait_time(float dt);
void free_window();

#endif
