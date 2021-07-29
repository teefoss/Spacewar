#ifndef window_h
#define window_h

#include <cstdlib>
#include <SDL2/SDL.h>

class Window
{
public:
    Window(const char * title = "",
           int w = 640,
           int h = 480,
           int x = SDL_WINDOWPOS_CENTERED,
           int y = SDL_WINDOWPOS_CENTERED,
           Uint32 flags = 0);
    ~Window();
      
    SDL_Window * SDL() { return window; };
    
    int getWidth(void);
    int getHeight(void);
    SDL_Rect getSize(void);
    void toggleFullscreen(void);
    bool isFullscreen(void) { return is_fullscreen; };
    
private:
    SDL_Window * window;
    bool is_fullscreen = false;
};

#endif // window_h
