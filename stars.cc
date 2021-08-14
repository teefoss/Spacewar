#include "stars.h"

#include "random.h"
#include "utility.h"
#include "defines.h"

StarField star_field = StarField(GAME_W, GAME_H);


StarField::StarField(int width, int height)
{
    static const DOS_Color star_colors[] = {
        DOS_BLUE,
        DOS_BRIGHT_BLUE,
        DOS_WHITE
    };

    num_stars = Random(700, 1100);
    star_store = new Star[num_stars];
    
    Star * s = star_store;
    for ( int i = 0; i < num_stars; i++, s++ ) {
        s->x = Random(0, width);
        s->y = Random(0, height);
        s->color = RANDOM_ELEMENT(star_colors);
        s->twinkle = Random(0, 200) == 0;
    }
}


void StarField::Draw(SDL_Renderer * renderer)
{
    static const DOS_Color twinkle_colors[] = { DOS_YELLOW, DOS_GRAY };
    
    Star * s = star_store;
    for ( int i = 0; i < num_stars; i++, s++ ) {
        if ( s->twinkle ) {
            DOS_SetColor(renderer, RANDOM_ELEMENT(twinkle_colors));
        } else {
            DOS_SetColor(renderer, s->color);
        }
        
        SDL_RenderDrawPoint(renderer, s->x, s->y);
    }
}
