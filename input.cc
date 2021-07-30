#include "input.h"
#include "utility.h"
#include "game.h"

#include <dos.h>

InputManager input;


void InputManager::init(SDL_Renderer * renderer)
{
    keys = SDL_GetKeyboardState(NULL);
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_GameControllerAddMappingsFromFile("gamecontroller.txt");
    
    for ( int i = 0; i < MAX_CONTROLLERS; i++ ) {
        controllers[i] = NULL;
    }
    
    console = DOS_NewConsole(renderer, 80, 5, DOS_MODE80);
    DOS_CSetCursorType(console, DOS_CURSOR_NONE);
    updateConsole();
}


void InputManager::updateConsole()
{
    int num_controllers = numControllers();
    int which_keyboard = 0;
    const char * kb_strings[2] = { "Keyboard (WASD)", "Keyboard (Arrows)" };
    
    DOS_ClearConsole(console);
    DOS_CSetBackground(console, DOS_TRANSPARENT);
    DOS_ClearBackground(console);
    
    DOS_CSetForeground(console, DOS_BRIGHT_WHITE);
    DOS_CPrintString(console, "Input Status\n");
    
    for ( int i = 0; i < MAX_PLAYERS; i++ ) {
        DOS_CSetForeground(console, DOS_WHITE);
        DOS_CPrintString(console, "Player %d: ", i + 1);
        if ( num_controllers ) {
            DOS_CSetForeground(console, DOS_BRIGHT_GREEN);
            DOS_CPrintString(console, "Controller Connected");
            --num_controllers;
        } else {
            if ( which_keyboard < KB_NUM_USERS ) {
                DOS_CSetForeground(console, DOS_BROWN);
                DOS_CPrintString(console, kb_strings[which_keyboard++]);
            } else {
                DOS_CSetBlink(console, true);
                DOS_CSetForeground(console, DOS_BRIGHT_RED);
                DOS_CPrintString(console, "No input connected");
                DOS_CSetBlink(console, false);
            }
        }
        DOS_CPrintString(console, "\n");
    }
}


void InputManager::renderConsole()
{
    int height = DOS_CGetHeight(console) * DOS_CGetMode(console);
    int x_offset = DOS_CHAR_WIDTH * 2;
    
    DOS_RenderConsole(console, MENU_MARGIN + x_offset, GAME_H - MENU_MARGIN - height);
}


void InputManager::disconnectControllers()
{
    for ( int i = 0; i < MAX_CONTROLLERS; i++ ) {
        if ( controllers[i] ) {
            SDL_GameControllerClose(controllers[i]);
            controllers[i] = NULL;
        }
    }
    
    game.setNumPlayers(game.numPlayers());
    updateConsole();
}


void InputManager::connectControllers()
{
    int num_joysticks = SDL_NumJoysticks();
    int j = 0; // index into controllers[]
    
    for ( int i = 0; i < num_joysticks; i++ ) {
    
        if ( SDL_IsGameController(i) ) {
            
            controllers[j] = SDL_GameControllerOpen(i);
            
            if ( controllers[j] == NULL ) {
                fprintf(stderr, "__func__: couldn't attached controller\n");
                continue;
            }

            const char * name = SDL_GameControllerNameForIndex(i);
            printf("connected controller %d (%s).\n", j + 1, name);
            
            if ( ++j == MAX_CONTROLLERS ) { // we only support 4
                return;
            }
        }
    }
    
    updateConsole();
}


void InputManager::reconnectControllers()
{
    disconnectControllers();
    connectControllers();
    updateConsole();
}


int InputManager::numControllers()
{
    int num = 0;
    for ( int i = 0; i < MAX_CONTROLLERS; i++ ) {
        if ( controllers[i] != NULL ) {
            num++;
        }
    }
    
    return num;
}


// controllers take precedence. if num players > num controllers, use kb
// - if controller: controller_num == player_num
InputState InputManager::getInputState(int player_num)
{
    InputState state;
    
    // WASD (0) or arrows (1)?
    int which_keyboard = player_num - numControllers();
    
    if ( which_keyboard < 0 ) {
        state = getControllerInputState(player_num);
    } else {
        state = getKeyboardInputState(which_keyboard);
    }
    
    return state;
}


InputState InputManager::getKeyboardInputState(int which_keyboard)
{
    InputState state;
    memset(&state, 0, sizeof(state));
    
    state.left   = keys[which_keyboard ? SDL_SCANCODE_LEFT  : SDL_SCANCODE_A];
    state.right  = keys[which_keyboard ? SDL_SCANCODE_RIGHT : SDL_SCANCODE_D];
    state.thrust = keys[which_keyboard ? SDL_SCANCODE_DOWN  : SDL_SCANCODE_S];
    state.shoot  = keys[which_keyboard ? SDL_SCANCODE_UP    : SDL_SCANCODE_W];
    state.shield = keys[which_keyboard ? SDL_SCANCODE_SLASH : SDL_SCANCODE_E];
        
    return state;
}


InputState InputManager::getControllerInputState(int controller_num)
{
    InputState state;
    memset(&state, 0, sizeof(state));
    
    SDL_GameController * c = controllers[controller_num];
    state.left   = SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_LEFT);
    state.right  = SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
    state.thrust = SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    state.shoot  = SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
    state.shield = SDL_GameControllerGetButton(c, SDL_CONTROLLER_BUTTON_Y);
    
    return state;
}
