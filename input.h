#ifndef input_h
#define input_h

#include <SDL2/SDL.h>
#include <dos.h>

#define NUM_CONTROLLERS 4
#define KEYB_USERS 2

// TODO: convert to DOS_Console

typedef struct
{
    unsigned left       : 1;
    unsigned right      : 1;
    unsigned forward    : 1;
    unsigned shoot      : 1;
    unsigned shield     : 1;
} InputState;

class Game;

struct InputManager
{
    const unsigned char * keys;
    SDL_GameController * controllers[NUM_CONTROLLERS];
    
    InputManager() { }
    void init(SDL_Renderer * renderer);
    
    void disconnectControllers();
    void connectControllers();
    void reconnectControllers();
    int numControllers();
    
    bool buttonPressed(int controller, SDL_GameControllerButton button);
    void processGameEvent(float dt);
    
    void renderConsole(void);
    
    InputState getInput(int player_num);
    
private:
    void updateConsole(void);
    DOS_Console * console;
};

extern InputManager input;

#endif /* input_h */
