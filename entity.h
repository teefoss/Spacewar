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

    virtual void draw(SDL_Renderer * renderer) = 0;
    virtual void update(float dt) = 0;
    virtual void contact(Entity * hit) = 0;
    virtual int size(void) = 0;
    
    void emitParticles(int count, int dos_color);
    void exertGravity(Entity * obj, float gravity, float dt);
    bool isColliding(Entity * other);
    void rotateByDegrees(float degrees);
    void loadTexture(const char * filename);
    const char * getTextureName(void);
    
    Vec2 velocity;
    EntityType type;
    boolean alive = true;
    
protected:
    void updatePosition(float dt, bool wrap);
    void drawSprite(SDL_Renderer * renderer, int cell_x = 0, int cell_y = 0);
    void setOrientation(Cardinal direction);
    Vec2 randomPointWithinRange(float range);

    Vec2 position;
    Vec2 orientation;
    float radius;
    float angle; // for renderering, orientation angle in degrees
    const char * texture_name; // (file name)
    SDL_Texture * texture;
};

#endif /* object_h */
