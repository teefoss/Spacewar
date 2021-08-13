#ifndef blackhole_h
#define blackhole_h

#include "entity.h"


class BlackHole : public Entity
{
public:
    BlackHole();
    
    void update(float dt) override;
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;
    int size(void) override;
    
    void setMove(bool value);
    bool isMoving() { return move; };
    
private:
    s16 move_timer;
    boolean move;
};

#endif /* blackhole_h */
