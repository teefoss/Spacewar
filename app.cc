#include "app.h"

#include "game.h"
#include "log.h"
#include "types.h"
#include "net.h"
#include "menu.h"

#include <string.h>

App * App::m_instance = NULL;

static void InitSDL()
{
    u32 flags = 0;
    flags |= SDL_INIT_VIDEO;
    flags |= SDL_INIT_AUDIO;
    flags |= SDL_INIT_GAMECONTROLLER;
    
    if ( SDL_Init(flags) < 0 ) {
        SDL_ERROR("SDL_Init failed");
    }
}

App::App(int argc, char ** argv)
{
    assert( m_instance == NULL && "Tried to init another instance of App");
        
    m_instance = this;
    m_args = (Args){ argc, argv };

    Log::create("log.txt");
    InitSDL();
    
    int scale = 2;
    m_window = SDL_CreateWindow(GAME_NAME,
                                0, 0,
                                GAME_W * scale, GAME_H * scale,
                                0);
    if ( m_window == NULL ) {
        SDL_ERROR("SDL_CreateWindow failed");
    }

    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    if ( m_renderer == NULL ) {
        SDL_ERROR("SDL_CreateRenderer failed");
    }
    
    SDL_RenderSetLogicalSize(m_renderer, GAME_W, GAME_H);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);

    InitNet();
    InitMenu();
    input.init(m_renderer);
    DOS_InitSound();

    game.init();
}


App::~App()
{
    game.quit();
    NetQuit();
    
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    
    Log::destroy();
}


void App::toggleFullscreen()
{
    if ( m_fullscreen ) {
        SDL_SetWindowFullscreen(m_window, 0);
    } else {
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    
    m_fullscreen = !m_fullscreen;
}


void App::processControllerButton(u8 button)
{
    if ( button == SDL_CONTROLLER_BUTTON_START ) {
        menu_is_open = !menu_is_open;
    }
    
    if ( menu_is_open ) {
        ProcessMenuControllerButton(button);
    }
}


void App::processKeyDown(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_ESCAPE:
            menu_is_open = !menu_is_open;
            DOS_QueueSound(1320, 50);
            DOS_QueueSound(1100, 50);
            DOS_PlayQueuedSound();
            break;
        case SDLK_BACKSLASH:
            toggleFullscreen();
            break;
        case SDLK_p:
            game.paused = !game.paused;
        default:
            break;
    }
}


void App::processEvents()
{
    SDL_Event event;
    
    while ( SDL_PollEvent(&event) ) {
        switch ( event.type ) {
            case SDL_QUIT:
                m_running = false;
                break;
            case SDL_KEYDOWN:
                if ( menu_is_open ) {
                    ProcessMenuKey(event.key.keysym.sym);
                } else {
                    processKeyDown(event.key.keysym.sym);
                }
                break;
            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                input.reconnectControllers();
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                processControllerButton(event.cbutton.button);
                break;
            default:
                break;
        }
    }
}


void App::run()
{
    //int last_ms = 0;
    int ticks = 0;
    
    while ( m_running ) {
        
        while ( !SDL_TICKS_PASSED(SDL_GetTicks(), ticks + 16) ) {
            SDL_Delay(1);
        }
        float dt = (SDL_GetTicks() - ticks) / 1000.0f;
        ticks = SDL_GetTicks();
        
        if (dt > 0.05f) { // TEMP
            dt = 0.05f;
        }
        
        processEvents();
        game.getPlayerInput();
        game.update(dt);
        game.draw(m_renderer);
    }
}


void App::quit()
{
    m_running = false;
}
