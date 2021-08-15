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

typedef struct
{
    Vec2        position;
    Cardinal    direction;
    DOS_Color   color;
    SDL_Rect    hud_rect;
} PlayerInfo;


class Player : public Entity
{
public:
    Player(int index);
    ~Player();

    void Update(float dt) override;
    void updateFromInputState(InputState input_state, float dt);
    void Draw(SDL_Renderer * renderer) override;
    void Contact(Entity * hit) override;
    int Size(void) override;
    
    bool IsActive(void);
    void Explode(int dos_color);
    void Thrust(float dt);
    void ShootBullet(void);
    void RotateByDegrees(float degrees);
    Vec2 NozzlePoint(void);
    Vec2 LaserEndPoint(void);
    void LaserPlayer(Player * other_player);
    void ExplosionSound(void);
    void RenderHUD(SDL_Renderer * renderer);
    
    u8 number;
    s8 num_lives = MAX_LIVES; // TODO: need a kill function
    s8 num_bullets = MAX_BULLETS;
    PowerupType powerup = POWERUP_NONE;
    s8 shield_strength = MAX_SHIELD_STRENGTH;
    boolean shield_up = false;
    
private:
    void ResetPosition(void);
    bool IsDead(void);
    bool IsRespawning(void);
    void EatPowerup(Powerup * powerup);
    void SetPowerupEffect(Powerup * powerup);
    void MakeHUDTexture(SDL_Renderer * renderer);

    s16 respawn_timer = 0;
    s16 shoot_cooldown_timer = 0;
    s16 bullet_recharge_timer = 0;
    s16 powerup_timer = 0; // powerup active if > 0
    SDL_Texture * hud_texture = NULL;
    SDL_Texture * bullet_texture = NULL;
};

extern const PlayerInfo player_info[MAX_PLAYERS];

#endif /* player_h */
