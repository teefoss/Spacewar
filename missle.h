#ifndef missle_h
#define missle_h

#include "entity.h"
#include "defines.h"

class Missle : public Entity
{
public:
    Missle();
    
    void Update(float dt) override;
    void Contact(Entity * hit) override;
    void Draw(SDL_Renderer * renderer) override;
    int  Size(void) override;
    void Explode(DOS_Color, u16 min_freq, u16 max_freq) override;
    
private:
    const float VELOCITY = 45.0f;
    const int INITIAL_RADIUS = GAME_W;
    const int MAX_DIST_FROM_PLAYER = GAME_W * 1.5;
    
    u32 TargetClosestPlayer();
    int sound_next_ms = 0;
    void PlayHomingSound(u32 dist_to_player);
};

#endif /* missle_h */
