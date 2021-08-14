#include "blackhole.h"
#include "utility.h"
#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "random.h"
#include "game.h"

#define BLACK_HOLE_FILE "black_hole.png"

BlackHole::BlackHole()
: Entity(ENTITY_BLACK_HOLE, VEC2_CENTER, BLACK_HOLE_RADIUS, BLACK_HOLE_FILE)
{
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
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * player = (Player *)hit;
            if ( player->IsActive() ) {
                player->num_lives--;
                player->Explode(DOS_RED);
                RandomizedSound(30, 200, 300);
            }
            break;
        }
        case ENTITY_BULLET: {
            Bullet * bullet = (Bullet *)hit;
            bullet->Explode(DOS_RED, 300, 400);
            break;
        }
        default:
            break;
    }
}
