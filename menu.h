// TODO: this is an epic mess

#ifndef menu_h
#define menu_h

#include "command.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <dos.h>
#include <vector>

#define MENU_NAME_LEN   256

class Game;

typedef enum {
    ITEM_NULL       = -1,
    ITEM_DISABLED   = 0,
    ITEM_ENABLED    = 1,
    ITEM_CYCLE      = 2,
    ITEM_TOGGLE     = 3,
} ItemState;

typedef enum
{
    CHOICE_NONE,
    CHOICE_SELECT,
} ChoiceType;

struct Menu;

struct MenuItem
{
    const char * name;
    ItemState state;
    int (*action)(void);
    int * value;
};

struct Menu
{
    char name[MENU_NAME_LEN];
    int num_items;
    MenuItem * items;
    int selected;
    Menu * previous_menu;
};

void InitMenu(void);
void DrawMenu(void);
bool ProcessMenuKey(SDL_Keycode key);
bool ProcessMenuControllerButton(uint16_t button);

extern bool menu_is_open;

#endif /* menu_h */
