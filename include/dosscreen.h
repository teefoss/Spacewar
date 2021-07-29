#ifndef dosscreen_h
#define dosscreen_h

#include "dosconsole.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DOS_Screen DOS_Screen;

void
DOS_InitScreen
(   const char * window_name,
    int console_w,
    int console_h,
    DOS_Mode text_style,
    int border_size );

void DOS_RenderScreen(void);
void DOS_SwitchPage(int new_page);
SDL_Window * DOS_GetWindow(void);
void DOS_ClearScreen(void);
void DOS_PrintChar(uint8_t ch);
void DOS_PrintString(const char * format, ...);
void DOS_GotoXY(int x, int y);
void DOS_SetBlink(bool blink);
void DOS_SetBorderColor(int color);
void DOS_SetForeground(int color);
void DOS_SetBackground(int color);

void DOS_SetFullscreen(bool fullscreen);
void DOS_ToggleFullscreen(void);
void DOS_SetScreenScale(int scale);

void DOS_SetScreenScale(int scale);
void DOS_IncreaseScreenScale(void);
void DOS_DecreaseScreenScale(void);

void DOS_SetCursorType(DOS_CursorType type);

float DOS_LimitFrameRate(int fps);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* dosscreen_h */
