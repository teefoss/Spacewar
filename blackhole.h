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
    
    void setMove(bool value);
    bool isMoving() { return move; };
    
private:
    int move_timer;
    bool move;
};

#endif /* blackhole_h */
