#ifndef player_h
#define player_h

#include "entity.h"
#include "storage.h"
#include "defines.h"
#include "powerup.h"
#include "input.h"

#include <vector>

// TODO: player state
struct PlayerState
{
    int time;
    PlayerState * next_state;
    
    virtual void draw(void) = 0;
    virtual void update(void) = 0;
    virtual void contact(void) = 0;
    virtual void processInput(void) = 0; // TODO: ?
};


class Player : public Entity
{
public:
    Player(int index);
    ~Player();

    void update(float dt) override;
    void updateFromInputState(InputState input_state, float dt);
    void draw(SDL_Renderer * renderer) override;
    void contact(Entity * hit) override;
    
    bool isActive(void);
    void explode(int dos_color);
    void thrust(float dt);
    void shootBullet(void);
    void rotateByDegrees(float degrees);
    Vec2 nozzlePoint(void);
    Vec2 laserEndPoint(void);
    void laserPlayer(Player * other_player);
    void explosionSound(void);
    void renderHUD(SDL_Renderer * renderer);
    
    u8 number;
    s8 num_lives; // TODO: need a kill function
    s8 num_bullets;
    DOS_Color color;
    PowerupType powerup;
    s8 shield_strength;
    bool shield_up;
    
private:
    void resetPosition(void);
    bool isDead(void);
    bool isRespawning(void);
    void eatPowerup(Powerup * powerup);
    void setPowerupEffect(Powerup * powerup);
    void makeHUDTexture(SDL_Renderer * renderer);

    s16 respawn_timer;
    s16 shoot_cooldown_timer;
    s16 bullet_recharge_timer;
    s16 powerup_timer; // powerup active if > 0

    Vec2 initial_position; // store this information elsewhere and look it up: pl num
    Cardinal initial_direction; // "
    
    SDL_Rect hud_rect; // store this elsewhere or calculate it based on pl num
};

#endif /* player_h */
