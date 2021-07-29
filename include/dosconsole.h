#ifndef dosconsole_h
#define dosconsole_h

#include "dostext.h"
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Appearance options for the console cursor
typedef enum
{
    DOS_CURSOR_NONE,    // Don't show the cursor.
    DOS_CURSOR_NORMAL,  // Bottom 20% cell
    DOS_CURSOR_FULL     // Cursor fills entire cell.
} DOS_CursorType;

// Structure describing one cell within the a console
typedef struct
{
    int character;  // ASCII value
    int fg_color;   // foreground color
    int bg_color;   // background color
    int blink;      // whether text blinks
} DOS_CharInfo;

typedef struct DOS_Console DOS_Console;

// Create a new console.
// renderer: the SDL_Renderer that will draw it
// w, h: the number of rows and column in the console (in characters cells)
DOS_Console * DOS_NewConsole
(   SDL_Renderer * renderer,
    int w,
    int h,
    DOS_Mode text_style );

void DOS_FreeConsole(DOS_Console * console);

// Clear the entire console. Foreground is reset to DOS_WHITE, background is
// reset to DOS_BLACK, and the cursor is returned to 0, 0.
void DOS_ClearConsole(DOS_Console * console);

// Set the background color of each console cell to the current
// background color.
void DOS_ClearBackground(DOS_Console * console);

// Render the console and x, y (window coordinates)
void DOS_RenderConsole(DOS_Console * console, int x, int y);

// Change the console's current foreground (text) and background colors.
void DOS_CSetForeground(DOS_Console * console, int color);
void DOS_CSetBackground(DOS_Console * console, int color);

// Print character at console's current cursor location
// with current foreground/background color.
void DOS_CPrintChar(DOS_Console * console, uint8_t ch);

// Put character at console's current cursor location
// with current foreground/background color.
// The following control characters are supported:
//  \n - place the cursor and the start of the next line
//  \t - move the cursor to the next tab stop
void DOS_CPrintString(DOS_Console * console, const char * format, ...);

// Move the cursor to position x, y.
void DOS_CGotoXY(DOS_Console * console, int x, int y);

// Get the current cursor position.
int  DOS_CGetX(DOS_Console * console);
int  DOS_CGetY(DOS_Console * console);

// Get the console size
int  DOS_CGetWidth(DOS_Console * console);
int  DOS_CGetHeight(DOS_Console * console);

DOS_Mode DOS_CGetMode(DOS_Console * console);

// Get/Set the character info at current cursor position.
DOS_CharInfo DOS_CGetChar(DOS_Console * console);
void DOS_CSetChar(DOS_Console * console, DOS_CharInfo * char_info);

// Set whether newly printed characters/strings should blink.
void DOS_CSetBlink(DOS_Console * console, bool blink);

void DOS_CSetTabSize(DOS_Console * console, int tab_size);
void DOS_CSetCursorType(DOS_Console * console, DOS_CursorType type);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* dosscreen_h */
