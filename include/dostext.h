#ifndef dostext_h
#define dostext_h

#include <SDL2/SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOS_CHAR_WIDTH    8
#define DOS_TRANSPARENT   -1

// Text mode options
typedef enum
{
    DOS_MODE40 = 8,  // wide characters (8 x 8)
    DOS_MODE80 = 16, // tall characters (8 x 16) default
} DOS_Mode;

int DOS_StringWidth(const char * format, ...);

// Basic rendering of Code Page 437 characters.
void DOS_RenderChar
(   SDL_Renderer * renderer,
    int x,
    int y,
    DOS_Mode mode,
    uint8_t character );
int DOS_RenderString
(   SDL_Renderer * renderer,
    int x,
    int y,
    DOS_Mode mode,
    const char * format, ... );

// Structure describing a character 'sprite sheet', made from
// a color palette. For faster drawing than DOS_RenderChar/DOS_RenderString
typedef struct dos_text DOS_Text;

DOS_Text * DOS_MakeText
(   SDL_Renderer * renderer, // the renderer that should draw it
    DOS_Mode mode,
    const SDL_Color * palette, // array of colors to create the sheet with
    int num_colors ); // color array count

// set the draw scale of given text sheet
void DOS_SetTextScale(DOS_Text * text, int scale);

// get scaled rectangle for character
SDL_Rect DOS_CharSize(DOS_Text * text);

void DOS_TPrintChar
(   DOS_Text * text,
    int x,
    int y,
    int fg_color,
    int bg_color,
    uint8_t ascii_char );

void
DOS_TPrintString
(   DOS_Text * text,
    int x,
    int y,
    int fg_color,
    int bg_color,
    const char * format, ... );

SDL_Renderer * DOS_GetTextRenderer(DOS_Text * text);
void DOS_DestroyText(DOS_Text * text);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* dostext_h */
