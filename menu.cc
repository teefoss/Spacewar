#include "menu.h"
#include "game.h"
#include "input.h"
#include "app.h"
#include "utility.h"

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
        
        switch ( item->display_type ) {
            case DISPLAY_INTEGER:
                DOS_CPrintString(con, "%s", item->name);
                DOS_CSetForeground(con, accent);
                DOS_CPrintString(con, " %d\n", *item->value);
                break;
            case DISPLAY_BOOLEAN:
                DOS_CPrintString(con, "%s", item->name);
                DOS_CSetForeground(con, *item->value ? accent : DOS_GRAY);
                DOS_CPrintString(con, *item->value ? "ON\n" : "OFF\n");
                break;
            case DISPLAY_INFO:
                DOS_CSetForeground(con, DOS_GREEN);
                DOS_CPrintString(con, "%s\n", item->name);
                break;
            default:
                DOS_CPrintString(con, "%s\n", item->name);
                break;
        }
    }
    
    DOS_RenderConsole(con, MENU_MARGIN, MENU_MARGIN);
}

void StartGame(void);
void QuitGame(void);
void ContinueGame(void);
void PreviousMenu(void);
void NextMenu(void);
void CyclePlayers(void);
void ToggleValue(void);

enum MenuList{
    MENU_MAIN,
    MENU_NEW_GAME,
    MENU_ABOUT,
    MENU_CONTROLS,
    MENU_COUNT
};

MenuItem new_game_items[] = {
    { "Start Game", ITEM_ENABLED,   StartGame,      0, NULL, DISPLAY_NORMAL },
    { "",           ITEM_NULL,      NULL,           0, NULL, DISPLAY_NORMAL },
    { "Players: ",  ITEM_ENABLED,   CyclePlayers,   0, NULL, DISPLAY_INTEGER },
    { "Hazards: ",  ITEM_ENABLED,   ToggleValue,    0, NULL, DISPLAY_BOOLEAN },
    { "Powerups: ", ITEM_ENABLED,   ToggleValue,    0, NULL, DISPLAY_BOOLEAN },
    { "Black Hole: ",ITEM_ENABLED,  ToggleValue,    0, NULL, DISPLAY_BOOLEAN },
    { "",           ITEM_NULL,      NULL,           0, NULL, DISPLAY_NORMAL },
    { "Back",       ITEM_ENABLED,   PreviousMenu,   0, NULL, DISPLAY_NORMAL },
};

MenuItem main_menu_items[] = {
    { "New Game",   ITEM_ENABLED,   NextMenu,       MENU_NEW_GAME, NULL, DISPLAY_NORMAL },
    { "Controls",   ITEM_ENABLED,   NextMenu,       MENU_CONTROLS, NULL, DISPLAY_NORMAL },
    { "About",      ITEM_ENABLED,   NextMenu,       MENU_ABOUT, NULL, DISPLAY_NORMAL },
    { "Quit",       ITEM_ENABLED,   QuitGame,       0, NULL, DISPLAY_NORMAL },
    { "",           ITEM_NULL,      NULL,           0, NULL, DISPLAY_NORMAL },
    { "Continue",   ITEM_DISABLED,  ContinueGame,   0, NULL, DISPLAY_NORMAL }
};

MenuItem about_menu_items[] = {
    { credits,      ITEM_DISABLED,  NULL, 	        0, NULL, DISPLAY_INFO },
    { "",           ITEM_NULL,      NULL,           0, NULL, DISPLAY_NORMAL },
    { "Back",       ITEM_ENABLED,   PreviousMenu,   0, NULL, DISPLAY_NORMAL }
};

MenuItem controls_menu_items[] = {
    { controls,     ITEM_DISABLED,  NULL,           0, NULL, DISPLAY_INFO },
    { "",           ITEM_NULL,      NULL,           0, NULL, DISPLAY_NORMAL },
    { "Back",       ITEM_ENABLED,   PreviousMenu,   0, NULL, DISPLAY_NORMAL }
};

Menu menus[MENU_COUNT] = {
    { GAME_NAME,  ARRAY_SIZE(main_menu_items),      main_menu_items,     0, -1 },
    { "New Game", ARRAY_SIZE(new_game_items),       new_game_items,      0, MENU_MAIN },
    { "About",    ARRAY_SIZE(about_menu_items),     about_menu_items,    2, MENU_MAIN },
    { "Controls", ARRAY_SIZE(controls_menu_items),  controls_menu_items, 2, MENU_MAIN }
};

#pragma mark -

void StartGame(void)
{
    main_menu_items[5].state = ITEM_ENABLED;
    current = &menus[MENU_MAIN];
    current->selected = 5;
    game.Start();
}


void QuitGame(void)
{
    App::Shared()->Quit();
}


void ContinueGame(void)
{
    if ( game.match_started ) {
        menu_is_open = false;
    }
}


void PreviousMenu(void)
{
    if ( current->previous_menu >= 0 ) {
        current = &menus[current->previous_menu];
    }
}


void NextMenu(void)
{
    current = &menus[current->items[current->selected].next_menu];
}


void CyclePlayers()
{
    int n = game.GetNumPlayers() + 1;
    
    if ( n > game.MaxPlayers() ) {
        n = 1;
    }
    
    game.SetNumPlayers(n);
}


void ToggleValue()
{
    int * value = current->items[current->selected].value;
    
    *value = !(*value);
}


void InitMenu()
{
    SDL_Renderer * renderer = App::Shared()->GetRenderer();
    con = DOS_NewConsole(renderer, 64, 20, DOS_MODE80);
    DOS_CSetCursorType(con, DOS_CURSOR_NONE);
    current = &menus[MENU_MAIN];
    
    new_game_items[2].value = &game.num_players;
    new_game_items[3].value = &game.hazards_on;
    new_game_items[4].value = &game.powerups_on;
    new_game_items[5].value = &game.black_hole_on;
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
        return;
    }
    MenuScrollSound();
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
        return;
    }
    MenuScrollSound();
}


static void ActionSound()
{
    DOS_QueueSound(800, 35);
    DOS_QueueSound(1200, 35);
    DOS_PlayQueuedSound();
}


void ProcessMenuKey(SDL_Keycode key)
{
    MenuItem * item = &current->items[current->selected];
    
    switch ( key ) {
        case SDLK_BACKSLASH:
            App::Shared()->ToggleFullscreen();
            break;
        case SDLK_DOWN:
            //MenuScrollSound();
            MenuDown();
            break;
        case SDLK_UP:
            //MenuScrollSound();
            MenuUp();
            break;
        case SDLK_RETURN:
            ActionSound();
            item->action();
            break;
        case SDLK_ESCAPE:
            ContinueGame();
        default:
            break;
    }
}


void ProcessMenuControllerButton(u16 button)
{
    switch ( button ) {
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            MenuUp();
            break;
        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            MenuDown();
            break;
        case SDL_CONTROLLER_BUTTON_A:
            ActionSound();
            current->items[current->selected].action();
            break;
        case SDL_CONTROLLER_BUTTON_START:
            ContinueGame();
            break;
        default:
            break;
    }
}
