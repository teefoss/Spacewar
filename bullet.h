#ifndef bullet_h
#define bullet_h

#include "entity.h"
#include "types.h"

#define BULLET_SOUND_LEN 10

class Bullet : public Entity
{
public:
    Bullet(Vec2 position, int who_shot);
    
    void update(float dt) override;
    void contact(Entity * hit) override;
    void draw(SDL_Renderer * renderer) override;
    int size(void) override;
    
    void explode(DOS_Color color, int freq_min, int freq_max);
    
private:
    u8 player_index;
};

#endif /* bullet_h */
