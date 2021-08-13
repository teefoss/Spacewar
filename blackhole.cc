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
    move_timer = 0;
    move = false;
}


int BlackHole::size()
{
    return (int)sizeof(*this);
}


void BlackHole::update(float dt)
{
    angle = RandomFloat(0.0f, 360.0f);
    
    // meander
    if ( move ) {
        if ( --move_timer < 0 ) {
            rotateByDegrees(Random(-10, 10));
            velocity = orientation * 2.0f;
            move_timer = MS_TO_TICKS(2000);
        }
        
        updatePosition(dt, true);
        
        // move back if too from the center
        
        Vec2 v = position - center;
        
        if ( v.length() > GAME_H / 2 ) {
            v.normalize();
            orientation = v;
            velocity = orientation * 2.0f;
            move_timer += MS_TO_TICKS(1000);
        }
    }
    
    for ( int i = 0; i < game.numPlayers(); i++ ) {
        Player * pl = game.players[i];
        if ( pl && pl->isActive() && pl->powerup != POWERUP_ZEROG ) {
            exertGravity(game.players[i], BLACK_HOLE_GRAVITY, dt);
        }
    }

}



void BlackHole::draw(SDL_Renderer * renderer)
{
    DOS_Color jet_colors[] = { DOS_RED, DOS_BRIGHT_RED };
    
    // draw jets
    
    for ( int i = 0; i < 5; i++ ) {
        Vec2 ray_end = Vec2(RandomFloat(10.0f, 20.0f), 0.0f);
        ray_end.rotateByDegrees(RANDOM_ANGLE);
        ray_end += position;
        DOS_SetColor(renderer, RANDOM_ELEMENT(jet_colors));
        DrawLine(renderer, position, ray_end);
    }
    
    Entity::drawSprite(renderer);
}



void BlackHole::setMove(bool value)
{
    move = value;
}



void BlackHole::contact(Entity * hit)
{
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * player = (Player *)hit;
            if ( player->isActive() ) {
                player->num_lives--;
                player->explode(DOS_RED);
                RandomizedSound(30, 200, 300);
            }
            break;
        }
        case ENTITY_BULLET: {
            Bullet * bullet = (Bullet *)hit;
            bullet->explode(DOS_RED, 300, 400);
            break;
        }
        default:
            break;
    }
}
