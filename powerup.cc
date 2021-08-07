#include "powerup.h"

#include "random.h"
#include "utility.h"
#include "defines.h"
#include <dos.h>

struct PowerupInfo
{
    int effect_time;
    bool disappears;
};

static PowerupInfo powerup_info[NUM_POWERUPS] = {
    { 0, 0 },
    { MS_TO_TICKS(15000), true }, // multi shot
    { 0, false }, // 1 up
    { 0, false }, // 3 up
    { MS_TO_TICKS(15000), true }, // laser
    { MS_TO_TICKS(20000), true }, // zero g
    { 0, false }, // extra bullets
    { MS_TO_TICKS(45000), true }, // show path
    { 0, false }, // nuke
};


Powerup::Powerup()
: Entity(ENTITY_POWERUP, Vec2(0, 0), 5.5f, "powerups.png")
{
    position.x = Random(HUD_MARGIN, GAME_W - HUD_MARGIN);
    position.y = Random(HUD_MARGIN, GAME_H - HUD_MARGIN);
    color = (DOS_Color)Random(DOS_GRAY, DOS_NUMCOLORS);
    //type = (PowerupType)Random(1, NUM_POWERUPS);
    type = POWERUP_SHOWPATH; // TEMP: debug
    
    PowerupInfo * info = &powerup_info[type];
    lifespan = info->disappears ? MS_TO_TICKS(25000) : -1;
    effect_time = info->effect_time;
}


void Powerup::update(float dt)
{
    (void)dt;
    
    if ( lifespan > 0 && --lifespan == 0 ) {
        alive = false;
    }
}



void Powerup::draw(SDL_Renderer * renderer)
{
    Entity::drawSprite(renderer, type);
}



void Powerup::contact(Entity * hit)
{
    switch ( hit->type ) {
        case ENTITY_BLACK_HOLE:
            alive = false; // just despawn it
            break;
        default:
            break;
    }
}
