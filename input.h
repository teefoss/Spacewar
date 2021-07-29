#ifndef input_h
#define input_h

#include <SDL2/SDL.h>

#define NUM_CONTROLLERS 4

// TODO: convert to DOS_Console

class Game;

struct InputManager
{
    const unsigned char * keys;
    SDL_GameController * controllers[NUM_CONTROLLERS];
    
    InputManager();
    void disconnectControllers();
    void connectControllers();
    void reconnectControllers();
    int numControllers();
    
    bool buttonPressed(int controller, SDL_GameControllerButton button);
    void processGameEvent(float dt);
};

#endif /* input_h */
