#ifndef input_h
#define input_h

#include "types.h"

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
    u8 left     : 1;
    u8 right    : 1;
    u8 thrust   : 1;
    u8 shoot    : 1;
    u8 shield   : 1;
} InputState;

class InputManager
{
public:
    InputManager() { }
    void init(SDL_Renderer * renderer);
    
    void reconnectControllers();
    int numControllers();
    void renderConsole(void);
    InputState getInputState(int player_num);
    
private:
    void updateConsole(void);
    void disconnectControllers(void);
    void connectControllers(void);
    InputState getKeyboardInputState(int which_keyboard);
    InputState getControllerInputState(int controller_num);

    const unsigned char * keys;
    SDL_GameController * controllers[MAX_CONTROLLERS];
    DOS_Console * console;
};

extern InputManager input;

#endif /* input_h */
