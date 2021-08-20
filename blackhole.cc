#include "blackhole.h"
#include "utility.h"
#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "random.h"
#include "game.h"

BlackHole::BlackHole()
: Entity(ENTITY_BLACK_HOLE, center, BLACK_HOLE_RADIUS, RES_TEXTURE_BLACKHOLE)
{
    min_frequency = 200;
    max_frequency = 400;
}


int BlackHole::Size()
{
    return (int)sizeof(*this);
}


void BlackHole::Update(float dt)
{
    angle = RandomFloat(0.0f, 360.0f);
        
    for ( int i = 0; i < game.GetNumPlayers(); i++ ) {
        Player * pl = game.players[i];
        if ( pl && pl->IsActive() && pl->powerup != POWERUP_ZEROG ) {
            ExertGravity(game.players[i], BLACK_HOLE_GRAVITY, dt);
        }
    }

}


void BlackHole::Draw(SDL_Renderer * renderer)
{
    DOS_Color jet_colors[] = { DOS_RED, DOS_BRIGHT_RED };
    
    // draw jets
    
    for ( int i = 0; i < 5; i++ ) {
        Vec2 ray_end = Vec2(RandomFloat(10.0f, 20.0f), 0.0f);
        ray_end.RotateByDegrees(RANDOM_ANGLE);
        ray_end += position;
        DOS_SetColor(renderer, RANDOM_ELEMENT(jet_colors));
        DrawLine(renderer, position, ray_end);
    }
    
    Entity::DrawSprite(renderer);
}


void BlackHole::Contact(Entity * hit)
{
    hit->Explode(DOS_RED, min_frequency, max_frequency);
}


void BlackHole::Explode(DOS_Color color, u16 min_freq, u16 max_freq)
{
    (void)color;
    (void)min_freq;
    (void)max_freq;
}
