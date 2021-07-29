#include "menu.h"
#include "defines.h"
#include "game.h"
#include "input.h"

#include <stdlib.h>
#include <dos.h>

bool menu_is_open = true;
static Menu * current;
static DOS_Console * con;

const char * credits = "Spacewar! was conceived in 1961 by Martin Graetz,\n"
"  Stephen Russell, and Wayne Wiitanen. It was first\n"
"  realized on the PDP-1 in 1962 by Stephen Russell,\n"
"  Peter Samson, Dan Edwards, and Martin Graetz,\n"
"  together with Alan Kotok, Steve Piner, and Robert\n"
"  A Saunders.\n\n"
"  2021 clone of Spacewar! by Thomas Foster";

const char * controls =
"           Controller    Keyboard (Max 2 players)\n"
"             ----------    ------------------------\n"
"                           PL 1       PL2\n"
"  Rotate:    D-PAD         A/D        LEFT/RIGHT\n"
"  Thrust:    R SHOULDER    S          DOWN\n"
"  Shoot:     A             W          UP\n";

void DrawMenu()
{
    const DOS_Color accent = DOS_BROWN;
    const DOS_Color selected = DOS_BRIGHT_WHITE;
    const DOS_Color text = DOS_WHITE;
    
    DOS_ClearConsole(con);
    DOS_CSetBackground(con, DOS_TRANSPARENT);
    DOS_ClearBackground(con);
    
    // menu title
    DOS_CSetForeground(con, accent);
    DOS_CPrintString(con, "  %s\n\n", current->name);
    
    // menu items
    DOS_CSetForeground(con, text);
    
    for ( int i = 0; i < current->num_items; i++ ) {
        
        MenuItem * item = &current->items[i];

        if ( i == current->selected ) {
            DOS_CSetForeground(con, selected);
            DOS_CPrintChar(con, DOS_RIGHTARROW);
            DOS_CSetForeground(con, selected);
            DOS_CPrintString(con, " ");
        } else {
            if ( item->state == ITEM_DISABLED ) {
                DOS_CSetForeground(con, DOS_GRAY);
            } else {
                DOS_CSetForeground(con, text);
            }
            DOS_CPrintString(con, "  ");
        }
        
        DOS_CPrintString(con, "%s", item->name);
        
        switch ( item->state ) {
            case ITEM_CYCLE:
                DOS_CSetForeground(con, accent);
                DOS_CPrintString(con, " %d\n", *item->value);
                break;
            case ITEM_TOGGLE:
                DOS_CSetForeground(con, *item->value ? accent : DOS_GRAY);
                DOS_CPrintString(con, *item->value ? "ON\n" : "OFF\n");
                break;
            default:
                DOS_CPrintString(con, "\n");
                break;
        }
    }
    
    DOS_RenderConsole(con, MENU_MARGIN, MENU_MARGIN);
}

int StartGame(void);
int QuitGame(void);
int ContinueGame(void);
int GoToNewGameMenu(void);
int GoToAboutMenu(void);
int GoToControlsMenu(void);
int PreviousMenu(void);
int CycleValue(void);
int ToggleValue(void);

static MenuItem new_game_items[8] = {
    { "Start Game", ITEM_ENABLED, StartGame, NULL },
    { "", ITEM_NULL, NULL, NULL },
    { "Players: ", ITEM_CYCLE, NULL, NULL },
    { "Aliens: ", ITEM_TOGGLE, NULL, NULL },
    { "Powerups: ", ITEM_TOGGLE, NULL, NULL },
    { "Black Hole Movement: ", ITEM_TOGGLE, NULL, NULL },
    { "", ITEM_NULL, NULL, NULL },
    { "Back", ITEM_ENABLED, PreviousMenu, NULL },
};

static MenuItem main_menu_items[6] = {
    { "New Game", ITEM_ENABLED, GoToNewGameMenu, NULL },
    { "Controls", ITEM_ENABLED, GoToControlsMenu, NULL },
    { "About", ITEM_ENABLED, GoToAboutMenu, NULL },
    { "Quit", ITEM_ENABLED, QuitGame, NULL },
    { "", ITEM_NULL, NULL, NULL },
    { "Continue", ITEM_DISABLED, ContinueGame, NULL }
};

static MenuItem about_menu_items[3] = {
    { credits, ITEM_DISABLED, NULL, NULL },
    { "", ITEM_NULL, NULL, NULL },
    { "Back", ITEM_ENABLED, PreviousMenu, NULL }
};

static MenuItem controls_menu_items[3] = {
    { controls, ITEM_DISABLED, NULL, NULL },
    { "", ITEM_NULL, NULL, NULL },
    { "Back", ITEM_ENABLED, PreviousMenu, NULL }
};

static Menu main_menu = { "Spacewar", 6, main_menu_items, 0, NULL };
static Menu new_game_menu = { "New Game", 8, new_game_items, 0, &main_menu };
static Menu about_menu = { "About", 3, about_menu_items, 2, &main_menu };
static Menu controls_menu = { "Controls", 3, controls_menu_items, 2, &main_menu };

#pragma mark -

int StartGame(void)
{
    main_menu_items[5].state = ITEM_ENABLED;
    current = &main_menu;
    current->selected = 5;
    game.start();
    
    return true;
}

int QuitGame(void)
{
    return false;
}

int ContinueGame(void)
{
    if ( game.match_started ) {
        menu_is_open = false;
    }
    return true;
}

int GoToNewGameMenu(void)
{
    current = &new_game_menu;
    return true;
}

int GoToAboutMenu(void)
{
    current = &about_menu;
    return true;
}

int GoToControlsMenu(void)
{
    current = &controls_menu;
    return true;
}

int PreviousMenu(void)
{
    current = current->previous_menu;
    return true;
}


void InitMenu()
{
    con = DOS_NewConsole(game.renderer, 64, 20, DOS_MODE80);
    DOS_CSetCursorType(con, DOS_CURSOR_NONE);
    current = &main_menu;
    
    new_game_items[2].value = &game.num_players;
    new_game_items[3].value = &game.aliens_on;
    new_game_items[4].value = &game.powerups_on;
    new_game_items[5].value = &game.black_hole_moves;
}


static void MenuScrollSound()
{
    DOS_SoundAsync(2000, 35);
}


void MenuDown()
{
    ++current->selected;

    if ( current->selected >= current->num_items ) {
        current->selected = 0;
    }
    
    // keep going past any NULL or disable menu items
    if (current->items[current->selected].state == ITEM_NULL
        || current->items[current->selected].state == ITEM_DISABLED )
    {
        MenuDown();
    }
}


// TODO: skip disabled etc
void MenuUp()
{
    --current->selected;

    if ( current->selected < 0 ) {
        current->selected = current->num_items - 1;
    }
    
    // keep going past any NULL or disable menu items
    if (current->items[current->selected].state == ITEM_NULL
        || current->items[current->selected].state == ITEM_DISABLED )
    {
        MenuUp();
    }
}

static void CycleNumPlayers()
{
    int n = game.numPlayers() + 1;
    
    if ( n > game.maxPlayers() ) {
        n = 1;
    }
    
    game.setNumPlayers(n);
}


bool ProcessMenuKey(SDL_Keycode key)
{
    MenuItem * item = &current->items[current->selected];
    
    switch ( key ) {
        case SDLK_BACKSLASH:
            game.window.toggleFullscreen();
            break;
        case SDLK_DOWN:
            MenuScrollSound();
            MenuDown();
            break;
        case SDLK_UP:
            MenuScrollSound();
            MenuUp();
            break;
        case SDLK_RETURN:
            DOS_QueueSound(800, 35);
            DOS_QueueSound(1200, 35);
            DOS_PlayQueuedSound();
            switch ( item->state ) {
                case ITEM_CYCLE:
                    CycleNumPlayers();
                    break;
                case ITEM_TOGGLE:
                    *item->value = !*item->value;
                    break;
                default:
                    return item->action();
            }
            break;
        case SDLK_ESCAPE:
            return ContinueGame();
        default:
            break;
    }
    
    return true;
}


bool ProcessMenuControllerButton(uint16_t button)
{
    switch ( button ) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            MenuUp();
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            MenuDown();
            break;
        case SDL_CONTROLLER_BUTTON_A:
            return current->items[current->selected].action();
            break;
        default:
            break;
    }
    
    return true;
}
