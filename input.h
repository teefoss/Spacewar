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

typedef enum
{
    INPUT_LEFT   = 0x01,
    INPUT_RIGHT  = 0x02,
    INPUT_THRUST = 0x04,
    INPUT_SHOOT  = 0x08,
    INPUT_SHIELD = 0x10
} InputFlags;

typedef u8 InputState;

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
    void disconnectControllers();
    void connectControllers();
    InputState getKeyboardInputState(int which_keyboard);
    InputState getControllerInputState(int controller_num);

    const unsigned char * keys;
    SDL_GameController * controllers[MAX_CONTROLLERS];
    DOS_Console * console;
};

extern InputManager input;

#endif /* input_h */
