#ifndef particle_h
#define particle_h

#include "vec2.h"
#include "storage.h"

#include <SDL2/SDL.h>

typedef struct
{
    Vec2        position;
    Vec2        velocity;
    int         lifespan;
    SDL_Color   color;
} Particle;

class ParticleSystem
{
public:
    ParticleSystem() {
        particle_store = Storage<Particle>();
    }
    void Spawn(Vec2 orig, Vec2 vel, int lifespan, SDL_Color & color);
    void Draw(SDL_Renderer * renderer);
    void Update(float dt);
    
private:
    Storage<Particle> particle_store;
};

extern ParticleSystem particles;

#endif /* particle_h */
