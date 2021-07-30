#ifndef input_h
#define input_h

#include <SDL2/SDL.h>
#include <dos.h>

#define MAX_CONTROLLERS 4

enum
{
    KB_WASD,
    KB_ARROWS,
    KB_NUM_USERS
};

typedef struct
{
    unsigned left       : 1;
    unsigned right      : 1;
    unsigned thrust     : 1;
    unsigned shoot      : 1;
    unsigned shield     : 1;
} InputState;

class Game;

struct InputManager
{
    const unsigned char * keys;
    SDL_GameController * controllers[MAX_CONTROLLERS];
    
    InputManager() { }
    void init(SDL_Renderer * renderer);
    
    void disconnectControllers();
    void connectControllers();
    void reconnectControllers();
    int numControllers();
    
    bool buttonPressed(int controller, SDL_GameControllerButton button);
    void processGameEvent(float dt);
    
    void renderConsole(void);
    
    InputState getInputState(int player_num);
    InputState getKeyboardInputState(int which_keyboard);
    InputState getControllerInputState(int controller_num);
    
private:
    void updateConsole(void);
    DOS_Console * console;
};

extern InputManager input;

#endif /* input_h */
