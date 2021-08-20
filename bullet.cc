#include "bullet.h"
#include "defines.h"
#include "utility.h"
#include "player.h"
#include "game.h"

#define BULLET_SOUND_LEN 10

static SDL_Rect bullet_bounds = {
    .x = (int)(-BULLET_RADIUS) * 2,
    .y = (int)(-BULLET_RADIUS) * 2,
    .w = GAME_W + ((int)BULLET_RADIUS * 2) * 2,
    .h = GAME_H + ((int)BULLET_RADIUS * 2) * 2
};


Bullet::Bullet(Vec2 position, int who_shot)
: Entity(ENTITY_BULLET, position, BULLET_RADIUS, "bullets.png")
{
    id = who_shot;
    min_frequency = 1200;
    max_frequency = 1600;
    color = game.players[who_shot]->GetColor();
}


int Bullet::Size()
{
    return (int)sizeof(*this);
}


void Bullet::Explode(DOS_Color color, u16 freq_min, u16 freq_max)
{
    EmitParticles(50, color);
    alive = false;
    RandomizedSound(10, min_frequency, max_frequency);
}


void Bullet::Update(float dt)
{
    UpdatePosition(dt, false);
    
    if ( !VecInRect(position, &bullet_bounds) ) {
        alive = false;
    }
}


void Bullet::Draw(SDL_Renderer * renderer)
{
    Entity::DrawSprite(renderer, id);
}


void Bullet::Contact(Entity * hit)
{
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * player = (Player *)hit;
            if ( id != player->id ) { // don't shoot yourself
                if ( player->shield_up ) {
                    Explode(color, min_frequency, max_frequency);
                } else {
                    player->Explode(player->GetColor(), 800, 1200);
                    alive = false;
                }
            }
            break;
        }
        case ENTITY_BULLET: {
            Explode(color, min_frequency, max_frequency);
            hit->Explode(hit->GetColor(), 0, 0);
        }
        default:
            break;
    }
}
