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

typedef enum
{
    ENTITY_BLACK_HOLE,
    ENTITY_PLAYER,
    ENTITY_BULLET,
    ENTITY_POWERUP,
} EntityType;

typedef struct
{
    void * buffer;
    size_t size;
} Data;

typedef struct
{
    Vec2 velocity;
    Vec2 position;
    Vec2 orientation;
    float angle;
    u8 alive;
} EntityData;

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

    virtual void draw(SDL_Renderer * renderer) = 0;
    virtual void update(float dt) = 0;
    virtual void contact(Entity * hit) = 0;
    virtual Data data(void) = 0;
    EntityData entityData(void);
    
    void emitParticles(int count, int dos_color);
    void exertGravity(Entity * obj, float gravity, float dt);
    bool isColliding(Entity * other);
    void rotateByDegrees(float degrees);
    
    Vec2 velocity;
    EntityType type;
    bool alive;
    
protected:
    void updatePosition(float dt, bool wrap);
    void drawSprite(SDL_Renderer * renderer, int sprite = 0);
    void setOrientation(Cardinal direction);
    Vec2 randomPointWithinRange(float range);

    Vec2 position;
    Vec2 orientation;
    float radius;
    float angle; // for renderering, orientation angle in degrees
    
private:
    const char * texture_name; // (file name)
    SDL_Texture * texture;
};

#endif /* object_h */
