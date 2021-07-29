#include "input.h"
#include "utility.h"
#include "game.h"

#include <dos.h>

static SDL_Scancode left[KEYB_USERS] = { SDL_SCANCODE_A, SDL_SCANCODE_LEFT };
static SDL_Scancode right[KEYB_USERS] = { SDL_SCANCODE_D, SDL_SCANCODE_RIGHT };
static SDL_Scancode thrust[KEYB_USERS] = { SDL_SCANCODE_S, SDL_SCANCODE_DOWN };
static SDL_Scancode shoot[KEYB_USERS] = { SDL_SCANCODE_W, SDL_SCANCODE_UP };
static SDL_Scancode shield[KEYB_USERS] = { SDL_SCANCODE_E, SDL_SCANCODE_SLASH };

InputManager input;

void InputManager::init(SDL_Renderer * renderer)
{
    keys = SDL_GetKeyboardState(NULL);
    SDL_GameControllerEventState(SDL_ENABLE);
    SDL_GameControllerAddMappingsFromFile("gamecontroller.txt");
    
    for ( int i = 0; i < NUM_CONTROLLERS; i++ ) {
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
            if ( which_keyboard < KEYB_USERS ) {
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
    for ( int i = 0; i < NUM_CONTROLLERS; i++ ) {
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
            
            if ( ++j == NUM_CONTROLLERS ) { // we only support 4
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
    for ( int i = 0; i < NUM_CONTROLLERS; i++ ) {
        if ( controllers[i] != NULL ) {
            num++;
        }
    }
    
    return num;
}



bool
InputManager::buttonPressed(int controller, SDL_GameControllerButton button)
{
    if ( controllers[controller] ) {
        return SDL_GameControllerGetButton(controllers[controller], button);
    }
    
    return false;
}

void InputManager::processGameEvent(float dt)
{
    float rotation_speed = PLAYER_ROTATION_SPEED * dt;
    
    for ( int i = 0; i < game.numPlayers(); i++ ) {
        
        Player * player = game.players[i];
        
        if (buttonPressed(i, SDL_CONTROLLER_BUTTON_DPAD_LEFT)
            || (i < KEYB_USERS && keys[left[i]]) )
        {
            player->rotateByDegrees(-rotation_speed);
        }
        
        if (buttonPressed(i, SDL_CONTROLLER_BUTTON_DPAD_RIGHT)
            || (i < KEYB_USERS && keys[right[i]]) )
        {
            player->rotateByDegrees(+rotation_speed);
        }

        if (buttonPressed(i, SDL_CONTROLLER_BUTTON_RIGHTSHOULDER)
            || (i < KEYB_USERS && keys[thrust[i]]) )
        {
            player->thrust(dt);
        }
                
        if (buttonPressed(i, SDL_CONTROLLER_BUTTON_A)
            || (i < KEYB_USERS && keys[shoot[i]]) )
        {
            if ( player->powerup != POWERUP_LASER ) {
                player->shootBullet(game.entities);
            }
        }
        
        if (buttonPressed(i, SDL_CONTROLLER_BUTTON_B)
            || (i < KEYB_USERS && keys[shield[i]]) )
        {
            if ( player->shield_strength ) {
                player->shield_up = true;
            }
        } else {
            player->shield_up = false;
        }
    }
}


InputState InputManager::getInput(int player_num)
{
    (void)player_num;
    InputState state;
    // determine which controller type
    return state;
}
