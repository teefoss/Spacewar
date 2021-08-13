#include "bullet.h"
#include "defines.h"
#include "utility.h"
#include "player.h"

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
    player_index = who_shot;
}


int Bullet::size()
{
    return (int)sizeof(*this);
}


void Bullet::explode(DOS_Color color, int freq_min, int freq_max)
{
    emitParticles(50, color);
    alive = false;
    RandomizedSound(10, freq_min, freq_max);
}


void Bullet::update(float dt)
{
    updatePosition(dt, false);
    
    if ( !VecInRect(position, &bullet_bounds) ) {
        alive = false;
    }
}


void Bullet::draw(SDL_Renderer * renderer)
{
    Entity::drawSprite(renderer, player_index);
}


void Bullet::contact(Entity * hit)
{
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * player = (Player *)hit;
            if ( player_index != player->number ) { // don't shoot yourself
                player->num_lives--;
                // TODO: 0 lives
                player->explode(player_info[player->number].color);
                RandomizedSound(30, 800, 1200);
                alive = false;
            }
            break;
        }
        case ENTITY_BULLET: {
            Bullet * other_bullet = (Bullet *)hit;
            if ( player_index != other_bullet->player_index ) {
                this->explode(DOS_RandomColor(), 1200, 1600);
                other_bullet->emitParticles(50, DOS_RandomColor());
                other_bullet->alive = false;
            }
            break;
        }
        default:
            break;
    }
}
