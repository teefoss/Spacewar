#include "entity.h"

#include "random.h"
#include "particle.h"
#include "utility.h"
#include "defines.h"
#include "resources.h"
#include "app.h"

Entity::Entity
(   EntityType type,
    Vec2 origin,
    float radius,
    const char * texture_name,
    Cardinal direction )
{
    this->type = type;
    this->position = origin;
    this->radius = radius;
    
    velocity.zero();
    setOrientation(direction);
    
    this->texture_name = texture_name;
    loadTexture(texture_name);
}



Entity::~Entity()
{
    ResourceManager::shared().destroyTexture(texture_name);
}


void Entity::loadTexture(const char * filename)
{
    SDL_Renderer * renderer = App::shared()->getRenderer();
    texture = ResourceManager::shared().getTexture(filename, renderer);
}


const char * Entity::getTextureName()
{
    return texture_name;
}


void Entity::setOrientation(Cardinal direction)
{
    // HACK: why doesn't this work if we start with { -1, 0 } (west)?
    this->orientation = Vec2(1.0f, 0.0f);
    angle = 0;
    rotateByDegrees((float)direction);
}


// just making sure 'angle' gets updated too
void Entity::rotateByDegrees(float degrees)
{
    angle += degrees;
    if ( angle < 0.0f ) {
        angle += 360.0f;
    } else if ( angle >= 360.0 ) {
        angle -= 360.0f;
    }
    orientation.rotateByDegrees(degrees);
    //orientation.normalize();
}



void Entity::drawSprite(SDL_Renderer * renderer, int cell_x, int cell_y)
{
    int diameter = (int)(radius * 2.0f);
    
    SDL_Rect dst;
    dst.x = (int)(position.x - radius);
    dst.y = (int)(position.y - radius);
    dst.w = diameter;
    dst.h = diameter;

    SDL_Rect src;
    src.x = diameter * cell_x;
    src.y = diameter * cell_y;
    src.w = diameter;
    src.h = diameter;
    
    SDL_RenderCopyEx(renderer, texture, &src, &dst, angle, NULL, SDL_FLIP_NONE);
}


// dos_color = -1: random DOS_Color
void Entity::emitParticles(int count, int dos_color)
{
    for ( int i = 0; i < count; i++ ) {
    
        SDL_Color sdl_color;
        if ( dos_color == RANDOM_COLORS ) {
            sdl_color = DOS_CGAColorToSDLColor(DOS_RandomColor());
        } else {
            sdl_color = DOS_CGAColorToSDLColor((DOS_Color)dos_color);
        }

        Vec2 par_origin = randomPointWithinRange(radius);
                    
        // send it on its way, randomly
        Vec2 par_vel;
        par_vel.x = RandomFloat(-10.0f, 10.0f);
        par_vel.y = RandomFloat(-10.0f, 10.0f);
        par_vel.rotateByDegrees(RANDOM_ANGLE);
        par_vel += velocity;
        
        int lifespan = Random(85, 145);
        
        particles.spawn(par_origin, par_vel, lifespan, sdl_color);
    }
}



bool Entity::isColliding(Entity * other)
{
    Vec2 v = position - other->position;
    float adjust = 0.8; // actual hitbox TODO: class member?
    
    return v.length() < radius * adjust + other->radius * adjust;
}



Vec2 Entity::randomPointWithinRange(float range)
{
    float dist = RandomFloat(0.0f, range);
    Vec2 pt = Vec2(dist, 0.0f);
    pt.rotateByDegrees(RANDOM_ANGLE);
    pt += position;

    return pt;
}



void Entity::exertGravity(Entity * obj, float gravity, float dt)
{
    Vec2 v = position - obj->position;
    v *= 1.0f / (v.length() * gravity) * dt;
//    v *= dt; // TODO: needed?
    
    obj->velocity += v;
}



void Entity::updatePosition(float dt, bool wrap)
{
    position += velocity * dt;
    
    if ( wrap ) {
        WrapPosition(&position, GAME_W, GAME_H);
    }
}
