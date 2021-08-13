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
    static App * shared() { return m_instance; }
    void run();
    void toggleFullscreen();
    Args getArgs() { return m_args; }
    SDL_Renderer * getRenderer() { return m_renderer; }
    
private:
    void processEvents(void);
    void processControllerButton(u8 button);
    void processKeyDown(SDL_Keycode key);
    void getPlayerInput(void);
    
    static App *    m_instance;
    SDL_Window *    m_window;
    SDL_Renderer *  m_renderer;
    int             m_argc;
    char **         m_argv;
    bool            m_fullscreen = true;
    bool            m_running = true;
    InputState      m_player_input[MAX_PLAYERS];
    Args            m_args;
};

#endif
