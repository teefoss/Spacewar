#ifndef app_h
#define app_h

#include "defines.h"
#include "input.h"

#include <assert.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

struct Args
{
    int count;
    char ** vector;
    
    const char * operator[](int index) const
    {
        assert(index <= count && "Args: bad index");
        return vector[index];
    }
};

class App
{
public:
    App(int argc = 0, char ** argv = NULL);
    ~App();
    App(App const&) = delete;
    void operator=(App const&) = delete;
    static App * Shared() { return m_instance; }
    void Run();
    void Quit();
    void ToggleFullscreen();
    Args GetArgs() { return m_args; }
    SDL_Renderer * GetRenderer() { return m_renderer; }
    int GetTicks() { return m_ticks; }
    
private:
    void InitWindow();
    void InitRenderer();
    void ProcessEvents(void);
    void ProcessControllerButton(u8 button);
    void ProcessKeyDown(SDL_Keycode key);
    
    static App *    m_instance;
    SDL_Window *    m_window;
    SDL_Renderer *  m_renderer;
    bool            m_fullscreen = true;
    bool            m_running = true;
    Args            m_args;
    int             m_ticks = 0;
};

#endif
