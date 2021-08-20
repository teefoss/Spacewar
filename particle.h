#ifndef particle_h
#define particle_h

#include "vec2.h"
#include "storage.h"
#include "types.h"

#include <SDL2/SDL.h>

typedef struct
{
    Vec2        position;
    Vec2        velocity;
    SDL_Color   color;
    s16         lifespan;
} Particle;

class ParticleSystem
{
public:
    void Spawn(Vec2 orig, Vec2 vel, s16 lifespan, SDL_Color color);
    void Draw(SDL_Renderer * renderer);
    void Update(float dt);
    
private:
    Storage<Particle> particle_store = Storage<Particle>();
};

extern ParticleSystem particles;

#endif /* particle_h */
