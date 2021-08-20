#include "missle.h"

#include "game.h"
#include "random.h"
#include "types.h"
#include "utility.h"
#include "app.h"

Missle::Missle()
: Entity(ENTITY_MISSLE, Vec2(0, 0), MISSLE_RADIUS, "missle.png")
{
    position = Vec2((float)INITIAL_RADIUS, 0.0f);
    position.RotateByDegrees(RANDOM_ANGLE);
    position += center;
    
    id = Random(0, game.GetNumPlayers()); // target
    color = DOS_RED;
    min_frequency = 100;
    max_frequency = 200;
}


void Missle::PlayHomingSound(u32 dist_to_player)
{
    int frequency = Map(dist_to_player, 0, MAX_DIST_FROM_PLAYER, 2000, 500);
    
    int ms = 50;
    DOS_QueueSound(frequency, ms);
    DOS_QueueSound(frequency + 50, ms);
    DOS_PlayQueuedSound();
}


u32 Missle::TargetClosestPlayer()
{
    int num_players = game.GetNumPlayers();
    u32 min_dist = U32_MAX;
    
    for ( int i = 0; i < num_players; i++ ) {
        
        if ( !game.players[i]->IsActive() ) {
            continue;
        }
        
        Vec2 v = position - game.players[i]->GetPosition();
        u32 dist = (u32)v.Length();
        
        if ( dist < min_dist ) {
            min_dist = dist;
            id = i;
        }
    }
    
    orientation = game.players[id]->GetPosition() - position;
    orientation.Normalize();
    velocity = orientation * VELOCITY;
    
    return min_dist;
}


void Missle::Update(float dt)
{
    u32 dist = TargetClosestPlayer();
        
    int ticks = App::Shared()->GetTicks();
    if ( ticks > sound_next_ms ) {
        PlayHomingSound(dist);
        int interval = Map(dist,
                           0, MAX_DIST_FROM_PLAYER,
                           MS_TO_TICKS(25), MS_TO_TICKS(4000));
        sound_next_ms = ticks + interval;
    }
    
    UpdatePosition(dt, false);
    
    if ( VecInRect(position, &visible_area) ) {
        ShootExhaustParticles(VELOCITY, DOS_BRIGHT_RED);
    }
}


void Missle::Explode(DOS_Color, u16 min_freq, u16 max_freq)
{
    EmitParticles(30, DOS_BRIGHT_RED);
    RandomizedSound(30, 100, 200);
    alive = false;
}


void Missle::Contact(Entity * hit)
{
    Explode(color, min_frequency, max_frequency);
    hit->Explode(hit->GetColor(), 0, 0);
}


void Missle::Draw(SDL_Renderer * renderer)
{
    Entity::DrawSprite(renderer);
}


int Missle::Size()
{
    return sizeof(*this);
}
