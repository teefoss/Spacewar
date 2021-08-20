#ifndef utility_h
#define utility_h

#include "vec2.h"
#include "types.h"

#include <stdlib.h>
#include <SDL2/SDL.h>

#define DrawLine(renderer, p1, p2) DrawLine_DDA(renderer, p1, p2)

#define CLAMP(x, min, max) if (x<min) {x=min;} else if (x>max) {x=max;}
#define SWAP(a, b) {auto c = a; a = b; b = c;}
#define MAX(a, b) ((a > b) ? (a) : (b))
#define MIN(a, b) ((a < b) ? (a) : (b))

#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

//
// the dumping ground
//

void ClampF(float * in, float min, float max);
int CalculateLargestWindowedScale(int height);
void SetFullscreen(SDL_Window * window, SDL_Renderer * renderer,
                   bool fs, int height);
void RandomizedSound(int len, int min_freq, int max_freq);

// geometry

bool VecInRect(Vec2 v, const SDL_Rect * r);
void PrintRect(SDL_Rect r);
void WrapPosition(Vec2 * position, int w, int h);
bool LineCircleIntersection(Vec2 p1, Vec2 p2,
                            Vec2 cp, double r/*, bool segment*/);

// math

int Map(int x, int in_min, int in_max, int out_min, int out_max);

// drawing

void SetColor(SDL_Renderer * renderer, const SDL_Color * color);
void DrawLine_DDA(SDL_Renderer * renderer, Vec2 p1, Vec2 p2);
void DrawLine_Bresenham(SDL_Renderer * renderer, Vec2 p1, Vec2 p2);
void DrawCircle(SDL_Renderer * renderer, u16 x0, u16 y0, u16 radius);
bool FlashInterval(int ms);

#endif /* utility_h */
