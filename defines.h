#ifndef defines_h
#define defines_h

#include <stdlib.h>

#define DEBUG_VISUAL            0
#define DEBUG_DATA              0

#define GAME_NAME               "Spacewar"
#define GAME_W                  640
#define GAME_H                  360
#define VEC2_CENTER             Vec2(GAME_W / 2, GAME_H / 2)

#define BLACK_HOLE_GRAVITY      0.1f // perfect, don't touch!!
#define BLACK_HOLE_RADIUS       4.0f

#define HUD_MARGIN              16
#define MENU_MARGIN             32

#define FPS                     60.0f
#define MS_PER_FRAME            16.66667
#define MS_TO_TICKS(x)          (int)(((float)x / MS_PER_FRAME) + 0.5f)
#define SEC_TO_TICKS(x)         MS_TO_TICKS(x * 1000)

#define MAX_PLAYERS             4
#define PLAYER_RADIUS           4.0f
#define PLAYER_MARGIN           48
#define RESPAWN_TICKS           (FPS * 3)
#define SHOT_DELAY              MS_TO_TICKS(250)
#define PLAYER_ROTATION_SPEED   175.0f

//#define PLAYER_THRUST           10.0f // perfect, don't touch!!
#define PLAYER_THRUST           11.0f // more perfect!?


#define MAX_LIVES               10

#define MAX_BULLETS             10
#define BULLET_RADIUS           1.5f
#define BULLET_MAX_VELOCITY     75.0f
#define BULLET_RECHARGE_TIME    MS_TO_TICKS(750)

#define SHIELD_RADIUS           9.0f
#define MAX_SHIELD_STRENGTH     MS_TO_TICKS(500)

#define RANDOM_COLORS           -1

#define ERROR(...)              fprintf(stderr, __VA_ARGS__); \
                                exit(EXIT_FAILURE);
#define SDL_ERROR(msg)          fprintf(stderr, msg ": %s\n", SDL_GetError()); \
                                exit(EXIT_FAILURE);

#endif /* defines_h */
