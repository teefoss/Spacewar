#include "app.h"

#include "game.h"
#include "log.h"
#include "types.h"
#include "net.h"
#include "menu.h"
#include "bullet.h"
#include "resources.h"

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


void App::InitWindow()
{
    int scale = 2;
    
    int x = SDL_WINDOWPOS_CENTERED;
    int y = SDL_WINDOWPOS_CENTERED;
    int w = GAME_W * scale;
    int h = GAME_H * scale;
    
    m_window = SDL_CreateWindow(GAME_NAME, x, y, w, h, 0);
    
    if ( m_window == NULL ) {
        SDL_ERROR("SDL_CreateWindow failed");
    }
}


void App::InitRenderer()
{
    m_renderer = SDL_CreateRenderer(m_window, -1, 0);
    
    if ( m_renderer == NULL ) {
        SDL_ERROR("SDL_CreateRenderer failed");
    }
    
    SDL_RenderSetLogicalSize(m_renderer, GAME_W, GAME_H);
    SDL_SetRenderDrawBlendMode(m_renderer, SDL_BLENDMODE_BLEND);
}


App::App(int argc, char ** argv)
{
    assert( m_instance == NULL && "Tried to init another instance of App");
        
    m_instance = this;
    m_args = (Args){ argc, argv };

    Log::Create("log.txt");
    InitSDL();
    InitWindow();
    InitRenderer();
    ResourceManager::Shared().Init(m_renderer);
    InitNet();
    InitMenu();
    input.Init(m_renderer);
    DOS_InitSound();
    game.Init();
    
#if 1
    LOG("size of player:  %zu bytes\n", sizeof(Player));
    LOG("size of bl hole: %zu bytes\n", sizeof(BlackHole));
    LOG("size of bullet:  %zu bytes\n", sizeof(Bullet));
    LOG("size of pup:     %zu bytes\n", sizeof(Powerup));
#endif
}


App::~App()
{
    game.Quit();
    NetQuit();
    
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
    
    Log::Destroy();
}


void App::ToggleFullscreen()
{
    if ( m_fullscreen ) {
        SDL_SetWindowFullscreen(m_window, 0);
    } else {
        SDL_SetWindowFullscreen(m_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    
    m_fullscreen = !m_fullscreen;
}


void App::ProcessControllerButton(u8 button)
{
    if ( button == SDL_CONTROLLER_BUTTON_START ) {
        menu_is_open = !menu_is_open;
    }
    
    if ( menu_is_open ) {
        ProcessMenuControllerButton(button);
    }
}


void App::ProcessKeyDown(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_ESCAPE:
            menu_is_open = !menu_is_open;
            DOS_QueueSound(1320, 50);
            DOS_QueueSound(1100, 50);
            DOS_PlayQueuedSound();
            break;
        case SDLK_BACKSLASH:
            ToggleFullscreen();
            break;
        case SDLK_p:
            game.paused = !game.paused;
        default:
            break;
    }
}


void App::ProcessEvents()
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
                    ProcessKeyDown(event.key.keysym.sym);
                }
                break;
            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                input.ReconnectControllers();
                break;
            case SDL_CONTROLLERBUTTONDOWN:
                ProcessControllerButton(event.cbutton.button);
                break;
            default:
                break;
        }
    }
}


void App::Run()
{
    int frame_ms = 0;
    
    if ( is_network_game ) {
        NetSync();
    }
    
    while ( m_running ) {
        ++m_ticks;
        
        while ( !SDL_TICKS_PASSED(SDL_GetTicks(), frame_ms + 16) ) {
            SDL_Delay(1);
        }
        float dt = (SDL_GetTicks() - frame_ms) / 1000.0f;
        frame_ms = SDL_GetTicks();
        
        if (dt > 0.05f) { // TEMP
            dt = 0.05f;
        }
        
        ProcessEvents();
        
        if ( is_network_game ) {
            game.NetUpdate(dt);
        } else {
            game.GetPlayerInput();
            game.Update(dt);
        }
        
        game.Draw(m_renderer);
    }
}


void App::Quit()
{
    LOG("Quitting Game\n");
    m_running = false;
}
