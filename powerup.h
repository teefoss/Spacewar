#ifndef powerup_h
#define powerup_h

#include "entity.h"
#include "types.h"

enum PowerupType : u8
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


class Powerup : public Entity
{
public:
    Powerup();
    void Update(float dt) override;
    void Draw(SDL_Renderer * renderer) override;
    void Contact(Entity * hit) override;
    int  Size(void) override;
    void Explode(DOS_Color color, u16 min_freq, u16 max_freq) override;

    PowerupType type;
    s16 effect_time; // how long the player has it for
    s16 lifespan; // how long before it disappears
    
private:
    DOS_Color color;
};

#endif /* powerup_h */
