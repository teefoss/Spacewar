#include "window.h"
#include <stdlib.h>


Window::Window(const char * title, int w, int h, int x, int y, Uint32 flags)
{
    window = SDL_CreateWindow(title, x, y, w, h, flags);
    
    if ( window == NULL ) {
        fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }
}


Window::~Window()
{
    SDL_DestroyWindow(window);
}


SDL_Rect Window::getSize()
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    SDL_Rect size = { 0, 0, w, h };
    
    return size;
}


int Window::getWidth()
{
    SDL_Rect size = getSize();
    return size.w;
}


int Window::getHeight()
{
    SDL_Rect size = getSize();
    return size.h;
}


void Window::toggleFullscreen()
{
    SDL_SetWindowFullscreen(window, is_fullscreen ? 0 :SDL_WINDOW_FULLSCREEN_DESKTOP);
    
    is_fullscreen = !is_fullscreen;
}
