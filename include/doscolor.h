#ifndef doscolor_h
#define doscolor_h

#include <SDL2/SDL.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define DOS_NUMCOLORS 16

typedef enum
{
    DOS_BLACK,
    DOS_BLUE,
    DOS_GREEN,
    DOS_CYAN,
    DOS_RED,
    DOS_MAGENTA,
    DOS_BROWN,
    DOS_WHITE,
    DOS_GRAY,
    DOS_BRIGHT_BLUE,
    DOS_BRIGHT_GREEN,
    DOS_BRIGHT_CYAN,
    DOS_BRIGHT_RED,
    DOS_BRIGHT_MAGENTA,
    DOS_YELLOW,
    DOS_BRIGHT_WHITE
} DOS_Color;

extern const SDL_Color dos_palette[DOS_NUMCOLORS];

void DOS_SetColor(SDL_Renderer * renderer, DOS_Color color);
void DOS_SetColorAlpha(SDL_Renderer * renderer, DOS_Color color, uint8_t alpha);
SDL_Color DOS_CGAColorToSDLColor(DOS_Color color);
DOS_Color DOS_RandomColor(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* doscolor_h */
