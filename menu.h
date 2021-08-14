#ifndef menu_h
#define menu_h

#include "defines.h"

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <dos.h>
#include <vector>

#define MENU_NAME_LEN   256

class Game;

typedef enum
{
    ITEM_NULL,
    ITEM_DISABLED,
    ITEM_ENABLED
} ItemState;

typedef enum
{
    DISPLAY_NORMAL,
    DISPLAY_INTEGER,
    DISPLAY_BOOLEAN,
    DISPLAY_INFO
} DisplayType;

typedef enum
{
    CHOICE_NONE,
    CHOICE_SELECT,
} ChoiceType;

struct Menu;

struct MenuItem
{
    const char *    name;
    ItemState       state;
    void            (*action)(void);
    int             next_menu;
    int *           value;
    DisplayType     display_type; // for value
};

struct Menu
{
    char            name[MENU_NAME_LEN];
    int             num_items;
    MenuItem *      items;
    int             selected;
    int             previous_menu;
};

void InitMenu(void);
void DrawMenu(void);
void ProcessMenuKey(SDL_Keycode key);
void ProcessMenuControllerButton(uint16_t button);

extern bool menu_is_open;

#endif /* menu_h */
