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
};

#endif /* blackhole_h */
