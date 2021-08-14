#ifndef stars_h
#define stars_h

#include "storage.h"
#include "types.h"

#include <dos.h>

#include <stdbool.h>
#include <SDL2/SDL.h>

typedef struct
{
    u16 x, y;
    DOS_Color color;
    bool twinkle;
} Star;

class StarField
{
public:
    StarField(int width, int height);
    ~StarField() { delete[] star_store; }
    void Init(int game_w, int game_h);
    void Draw(SDL_Renderer * renderer);
    
private:
    Star * star_store;
    int num_stars;
};

extern StarField star_field;

#endif /* stars_h */
