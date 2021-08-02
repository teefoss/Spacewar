#include "bullet.h"
#include "defines.h"
#include "utility.h"
#include "player.h"

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


Data Bullet::data()
{
    Data d;
    d.buffer = malloc(sizeof(BulletData));
    d.size = sizeof(BulletData);

    BulletData * buf = (BulletData *)d.buffer;
    buf->entity_data = Entity::entityData();
    buf->player_index = player_index;
    
    return d;
}


void Bullet::draw(SDL_Renderer * renderer)
{
    Entity::drawSprite(renderer, player_index);
}



void Bullet::update(float dt)
{
    updatePosition(dt, false);
    
    if ( !VecInRect(position, &bullet_bounds) ) {
        alive = false;
    }
}



void Bullet::contact(Entity * hit)
{
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * player = (Player *)hit;
            if ( player_index != player->number ) { // don't shoot yourself
                player->num_lives--;
                // TODO: 0 lives
                player->explode(player->color);
                RandomizedSound(30, 800, 1200);
                alive = false;
            }
            break;
        }
        case ENTITY_BULLET: {
            Bullet * other_bullet = (Bullet *)hit;
            if ( player_index != other_bullet->player_index ) {
                this->emitParticles(50, DOS_RandomColor());
                other_bullet->emitParticles(50, DOS_RandomColor());
                this->alive = false;
                other_bullet->alive = false;
                RandomizedSound(sound_len, 1200, 1600);
            }
            break;
        }
        default:
            break;
    }
}
