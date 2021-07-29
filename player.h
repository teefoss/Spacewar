#ifndef player_h
#define player_h

#include "entity.h"
#include "storage.h"
#include "defines.h"
#include "powerup.h"

#include <vector>

struct PlayerState
{
    int time;
    PlayerState * next_state;
    
    virtual void draw(void) = 0;
    virtual void update(void) = 0;
    virtual void contact(void) = 0;
    virtual void processInput(void) = 0; // TODO: ?
};

class Game;

class Player : public Entity
{
public:
    Player(int index);
    ~Player();
    
    void update(float dt) override;
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;
    bool isActive(void);
    void explode(int dos_color);
    void thrust(float dt);
    void shootBullet(Storage<Entity *> &entities);
    void rotateByDegrees(float degrees);
    Vec2 nozzlePoint(void);
    Vec2 laserEndPoint(void);
    void laserPlayer(Player * other_player);
    void explosionSound(void);
    void renderHUD(SDL_Renderer * renderer);
    
    int number;
    int num_lives; // TODO: need a kill function
    int num_bullets;
    DOS_Color color;
    PowerupType powerup;
    int shield_strength;
    bool shield_up;
    
private:
    void resetPosition(void);
    bool isDead(void);
    bool isRespawning(void);
    void eatPowerup(Powerup * powerup);
    void setPowerupEffect(Powerup * powerup);
    void makeHUDTexture(SDL_Renderer * renderer);

    int respawn_timer;
    int shoot_cooldown_timer;
    int bullet_recharge_timer;
    int powerup_timer; // powerup active if > 0

    Vec2 initial_position;
    Cardinal initial_direction;
    
    SDL_Rect hud_rect;
    SDL_Texture * hud_texture;
};

#endif /* player_h */
