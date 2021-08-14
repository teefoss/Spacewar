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
    void Init(SDL_Renderer * renderer);
    
    void ReconnectControllers();
    int NumControllers();
    void RenderConsole(void);
    InputState GetInputState(int player_num);
    
private:
    void UpdateConsole(void);
    void DisconnectControllers(void);
    void ConnectControllers(void);
    InputState GetKeyboardInputState(int which_keyboard);
    InputState GetControllerInputState(int controller_num);

    const unsigned char * keys;
    SDL_GameController * controllers[MAX_CONTROLLERS];
    DOS_Console * console;
};

extern InputManager input;

#endif /* input_h */
