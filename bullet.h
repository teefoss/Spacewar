#ifndef bullet_h
#define bullet_h

#include "entity.h"

typedef struct
{
    EntityData entity_data;
    u8 player_index;
} BulletData;

class Bullet : public Entity
{
public:
    Bullet(Vec2 position, int who_shot);
    
    void update(float dt) override;
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;
    Data data(void) override;
    
    const int sound_len = 10;
    
private:
    int player_index;
};

#endif /* bullet_h */
