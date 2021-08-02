#include "blackhole.h"
#include "utility.h"
#include "defines.h"
#include "player.h"
#include "bullet.h"
#include "random.h"

#define BLACK_HOLE_FILE "black_hole.png"

BlackHole::BlackHole()
: Entity(ENTITY_BLACK_HOLE, VEC2_CENTER, BLACK_HOLE_RADIUS, BLACK_HOLE_FILE)
{
    move_timer = 0;
    move = false;
}

Data BlackHole::data()
{
    Data d;
    d.buffer = malloc(sizeof(BlackHoleData));
    d.size = sizeof(BlackHoleData);

    BlackHoleData * buf = (BlackHoleData *)d.buffer;
    buf->entity_data = Entity::entityData();
    buf->move_timer = move_timer;
    buf->move = move;
    
    return d;
}

//typedef struct
//{
//    Vec2 velocity;
//    Vec2 position;
//    Vec2 orientation;
//    float angle;
//    u8 alive;
//} EntityData;


BlackHole::BlackHole(Data * data)
: Entity(ENTITY_BLACK_HOLE, Vec2(0, 0), BLACK_HOLE_RADIUS, BLACK_HOLE_FILE)
{
    BlackHoleData * buf = (BlackHoleData *)data->buffer;
    
    velocity = buf->entity_data.velocity;
    position = buf->entity_data.position;
    orientation = buf->entity_data.orientation;
    angle = buf->entity_data.angle;
    alive = buf->entity_data.alive;
    
    
    move_timer = buf->move_timer;
    move = buf->move;
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
        
        Vec2 center = Vec2(GAME_W / 2, GAME_H / 2);
        Vec2 v = position - center;
        
        if ( v.length() > GAME_H / 2 ) {
            v.normalize();
            orientation = v;
            velocity = orientation * 2.0f;
            move_timer += MS_TO_TICKS(1000);
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
            bullet->emitParticles(50, DOS_RED);
            bullet->alive = false;
            RandomizedSound(bullet->sound_len, 300, 400);
            break;
        }
        default:
            break;
    }
}
