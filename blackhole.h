#ifndef blackhole_h
#define blackhole_h

#include "entity.h"


class BlackHole : public Entity
{
public:
    BlackHole();
    
    void Update(float dt) override;
    void Draw(SDL_Renderer * renderer) override;
    void Contact(Entity * hit) override;
    int Size(void) override;
    void Explode(DOS_Color color, u16 min_freq, u16 max_freq) override { }
};

#endif /* blackhole_h */
