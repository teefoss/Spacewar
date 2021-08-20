#ifndef bullet_h
#define bullet_h

#include "entity.h"
#include "types.h"

#define BULLET_SOUND_LEN 10

class Bullet : public Entity
{
public:
    Bullet(Vec2 position, int who_shot);
    
    void Update(float dt) override;
    void Contact(Entity * hit) override;
    void Draw(SDL_Renderer * renderer) override;
    int Size(void) override;
    void Explode(DOS_Color color, u16 freq_min, u16 freq_max) override;
};

#endif /* bullet_h */
