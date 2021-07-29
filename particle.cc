#include "particle.h"
#include "utility.h"
#include "defines.h"

ParticleSystem particles = ParticleSystem();


void ParticleSystem::spawn(Vec2 pos, Vec2 vel, int lifespan, SDL_Color & color)
{
    Particle p = { pos, vel, lifespan, color };
    particle_store.append(p);
}


void ParticleSystem::draw(SDL_Renderer * renderer)
{
    Particle * p = particle_store;
    for ( unsigned i = 0; i < particle_store.count(); i++, p++ ) {
        SetColor(renderer, &p->color);
        SDL_RenderDrawPoint(renderer, p->position.x, p->position.y);
    }
}


void ParticleSystem::update(float dt)
{
    Particle * p = particle_store;
    for ( unsigned i = 0; i < particle_store.count(); i++, p++ ) {
        p->position += p->velocity * dt;
        WrapPosition(&p->position, GAME_W, GAME_H);
        
        if ( --p->lifespan < 0 ) {
            particle_store.remove(i);
        }
    }
}
