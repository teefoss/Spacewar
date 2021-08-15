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
    id = who_shot;
}


int Bullet::Size()
{
    return (int)sizeof(*this);
}


void Bullet::Explode(DOS_Color color, int freq_min, int freq_max)
{
    EmitParticles(50, color);
    alive = false;
    RandomizedSound(10, freq_min, freq_max);
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
                    alive = false;
                    this->Explode(player_info[id].color, 1600, 2000);
                } else {
                    player->num_lives--;
                    // TODO: 0 lives
                    player->Explode(player_info[player->id].color);
                    RandomizedSound(30, 800, 1200);
                    alive = false;
                }
            }
            break;
        }
        case ENTITY_BULLET: {
            Bullet * other_bullet = (Bullet *)hit;
            if ( id != other_bullet->id ) {
                this->Explode(DOS_RandomColor(), 1200, 1600);
                other_bullet->EmitParticles(50, DOS_RandomColor());
                other_bullet->alive = false;
            }
            break;
        }
        default:
            break;
    }
}
