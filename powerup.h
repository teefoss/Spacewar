#ifndef powerup_h
#define powerup_h

#include "entity.h"

enum PowerupType
{
    POWERUP_NONE,
    POWERUP_MULTISHOT,
    POWERUP_ONEUP,
    POWERUP_THREEUP,
    POWERUP_LASER,
    POWERUP_ZEROG,
    POWERUP_EXTRABULLETS,
    POWERUP_SHOWPATH,
    POWERUP_NUKE,
    NUM_POWERUPS
};

// TODO: nuke - everything explodes


class Powerup : public Entity
{
public:
    Powerup();
    void update(float dt) override;
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;

    PowerupType type;
    int effect_time; // how long the player has it for
    int lifespan; // how long before it disappears
    
private:
    DOS_Color color;
};

#endif /* powerup_h */
