#include "particle.h"
#include "utility.h"
#include "defines.h"

ParticleSystem particles = ParticleSystem();


void ParticleSystem::Spawn(Vec2 pos, Vec2 vel, s16 lifespan, SDL_Color color)
{
    Particle p = {
        .position   = pos,
        .velocity   = vel,
        .color      = color,
        .lifespan   = lifespan
    };
    
    particle_store.Insert(p);
}


void ParticleSystem::Draw(SDL_Renderer * renderer)
{
    Particle * p = particle_store;
    for ( unsigned i = 0; i < particle_store.Count(); i++, p++ ) {
        SetColor(renderer, &p->color);
        SDL_RenderDrawPoint(renderer, p->position.x, p->position.y);
    }
}


void ParticleSystem::Update(float dt)
{
    Particle * p = particle_store;
    for ( unsigned i = 0; i < particle_store.Count(); i++, p++ ) {
        p->position += p->velocity * dt;
        WrapPosition(&p->position, GAME_W, GAME_H);
        
        if ( --p->lifespan <= 0 ) {
            particle_store.Remove(i);
        }
    }
}
