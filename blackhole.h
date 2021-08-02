#ifndef blackhole_h
#define blackhole_h

#include "entity.h"

typedef struct
{
    EntityData entity_data;
    u16 move_timer;
    u8 move;
} BlackHoleData;

class BlackHole : public Entity
{
public:
    BlackHole();
    BlackHole(Data * data);
    
    void update(float dt) override;
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;
    Data data(void) override;
    
    void setMove(bool value);
    bool isMoving() { return move; };
    
private:
    int move_timer;
    bool move;
};

#endif /* blackhole_h */
