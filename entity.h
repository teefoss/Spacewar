#ifndef object_h
#define object_h

#include "vec2.h"
#include "particle.h"
#include "types.h"

#include <dos.h>

typedef enum
{
    CARDINAL_EAST       = 0,
    CARDINAL_SOUTHEAST  = 45,
    CARDINAL_SOUTH      = 90,
    CARDINAL_SOUTHWEST  = 135,
    CARDINAL_WEST       = 180,
    CARDINAL_NORTHWEST  = 225,
    CARDINAL_NORTH      = 270,
    CARDINAL_NORTHEAST  = 315
} Cardinal;

enum EntityType : u8
{
    ENTITY_BLACK_HOLE,
    ENTITY_PLAYER,
    ENTITY_BULLET,
    ENTITY_POWERUP,
};


class Entity
{
public:
    Entity
    (   EntityType type,
        Vec2 origin,
        float radius,
        const char * texture_name,
        Cardinal direction = CARDINAL_EAST );
    virtual ~Entity();

    virtual void Draw(SDL_Renderer * renderer) = 0;
    virtual void Update(float dt) = 0;
    virtual void Contact(Entity * hit) = 0;
    virtual int Size(void) = 0;
    
    void EmitParticles(int count, int dos_color);
    void ExertGravity(Entity * obj, float gravity, float dt);
    bool IsColliding(Entity * other);
    void Rotate(float degrees);
    void LoadTexture(void);
    float GetHitbox() { return radius * hitbox_adjust; }
    Vec2 GetPosition() { return position; }

    u8 id;
    Vec2 velocity;
    EntityType type;
    boolean alive = true;
    
protected:
    void UpdatePosition(float dt, bool wrap);
    void DrawSprite(SDL_Renderer * renderer, int cell_x = 0, int cell_y = 0);
    void SetOrientation(Cardinal direction);
    Vec2 RandomPointWithinRange(float range);

    Vec2 position;
    Vec2 orientation;
    float radius;
    float hitbox_adjust = 1.0f;
    float angle; // for renderering, orientation angle in degrees
    char texture_name[100]; // (file name)
    SDL_Texture * texture;
};

#endif /* object_h */
