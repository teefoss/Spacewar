#include "player.h"

#include "random.h"
#include "utility.h"
#include "resources.h"
#include "bullet.h"
#include "particle.h"
#include "game.h"
#include "app.h"

#include <dos.h>

static const int player_right = GAME_W - PLAYER_MARGIN;
static const int player_bottom = GAME_H - PLAYER_MARGIN;
static const int player_diameter = PLAYER_RADIUS * 2;

#define HUD_WIDTH   (MAX_LIVES * player_diameter + MAX_LIVES * 2)
#define HUD_HEIGHT  (player_diameter + 10)

const PlayerInfo player_info[MAX_PLAYERS] =
{
    // PLAYER 1 [0] UPPER LEFT
    {
        .position = Vec2(PLAYER_MARGIN, PLAYER_MARGIN),
        .direction = CARDINAL_SOUTHEAST,
        .color = DOS_BRIGHT_GREEN,
        .hud_rect = { HUD_MARGIN, HUD_MARGIN, HUD_WIDTH, HUD_HEIGHT }
    },
    // PLAYER 1 [1] LOWER RIGHT
    {
        .position = Vec2(player_right, player_bottom),
        .direction = CARDINAL_NORTHWEST,
        .color = DOS_YELLOW,
        .hud_rect = {
            GAME_W - HUD_MARGIN - HUD_WIDTH,
            GAME_H - HUD_MARGIN - HUD_HEIGHT,
            HUD_WIDTH,
            HUD_HEIGHT
        }
    },
    // PLAYER 3 [2] LOWER LEFT
    {
        .position = Vec2(PLAYER_MARGIN, player_bottom),
        .direction = CARDINAL_NORTHEAST,
        .color = DOS_BRIGHT_CYAN,
        .hud_rect = {
            HUD_MARGIN,
            GAME_H - HUD_MARGIN - HUD_HEIGHT,
            HUD_WIDTH,
            HUD_HEIGHT
        }
    },
    // PLAYER 4 [3] UPPER RIGHT
    {
        .position = Vec2(player_right, PLAYER_MARGIN),
        .direction = CARDINAL_SOUTHWEST,
        .color = DOS_BRIGHT_MAGENTA,
        .hud_rect = {
            GAME_W - HUD_MARGIN - HUD_WIDTH,
            HUD_MARGIN,
            HUD_WIDTH,
            HUD_HEIGHT
        }
    },
};



Player::Player(int index)
: Entity(ENTITY_PLAYER, Vec2(0, 0), PLAYER_RADIUS, "ships.png")
{
    id = index;
    hitbox_adjust = 0.8f;
    
    ResetPosition();
        
    SDL_Renderer * renderer = App::Shared()->GetRenderer();
    hud_texture = SDL_CreateTexture(renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    HUD_WIDTH,
                                    HUD_HEIGHT);
    if ( hud_texture == NULL ) {
        fprintf(stderr, "could not create ship HUD texture\n");
        exit(EXIT_FAILURE);
    }
    
    SDL_SetTextureBlendMode(hud_texture, SDL_BLENDMODE_BLEND);
    
    bullet_texture = ResourceManager::Shared().GetTexture("bullets.png");
}


Player::~Player()
{
    SDL_DestroyTexture(hud_texture);
    ResourceManager::Shared().ReleaseTexture("bullets.png");
}


int Player::Size()
{
    return (int)sizeof(*this);
}


bool Player::IsColliding(Entity * other)
{
    Vec2 v = position - other->GetPosition();
    float my_radius = shield_up ? SHIELD_RADIUS : GetHitbox();
    
    return v.Length() < my_radius + other->GetHitbox();
}


void Player::MakeHUDTexture(SDL_Renderer * renderer)
{
    SDL_SetRenderTarget(renderer, hud_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    
    // lives
    
    const int margin = 2;
    const int ship_diam = (int)(radius * 2.0f);
    SDL_Rect src = { id * ship_diam, 0, ship_diam, ship_diam };
    SDL_Rect dst = { 0, 0, ship_diam, ship_diam };
    
    for ( int i = 0; i < num_lives; i++ ) {
        SDL_RenderCopyEx(renderer, texture, &src, &dst, 270, NULL, SDL_FLIP_NONE);
        dst.x += ship_diam + margin;
    }
    
    // shield strength
        
    SDL_Rect bar[2];
    for ( int i = 0; i < 2; i++ ) {
        bar[i].x = 0;
        bar[i].y = ship_diam + margin;
        bar[i].w = HUD_WIDTH - margin;
        bar[i].h = 3;
    }
    bar[1].w *= (float)shield_strength / (float)MAX_SHIELD_STRENGTH;

    DOS_SetColor(renderer, DOS_GRAY);
    SDL_RenderFillRect(renderer, &bar[0]);
    DOS_SetColor(renderer, player_info[id].color);
    SDL_RenderFillRect(renderer, &bar[1]);
    
    // bullets
    
    const int bullet_diam = (int)(BULLET_RADIUS * 2.0f);
    src = (SDL_Rect){ id * bullet_diam, 0, bullet_diam, bullet_diam };
    dst = (SDL_Rect){ 0, bar[0].y + bar[0].h + margin, bullet_diam, bullet_diam };
    
    DOS_SetColor(renderer, player_info[id].color);
    for ( int i = 0; i < num_bullets; i++ ) {
        SDL_RenderCopy(renderer, bullet_texture, &src, &dst);
        dst.x += bullet_diam + margin;
    }

    SDL_SetRenderTarget(renderer, NULL);
}



void Player::RenderHUD(SDL_Renderer * renderer)
{
    MakeHUDTexture(renderer); // TODO: only as needed
    
    SDL_RendererFlip flip;
    
    if ( id == 1 || id == 3 ) {
        flip = SDL_FLIP_HORIZONTAL;
    } else {
        flip = SDL_FLIP_NONE;
    }

    const SDL_Rect * dst = &player_info[id].hud_rect;
    SDL_RenderCopyEx(renderer, hud_texture, NULL, dst, 0, NULL, flip);
}


void Player::ExplosionSound()
{
    RandomizedSound(60, 400, 1600);
}


void Player::RotateByDegrees(float degrees)
{
    float new_degrees = degrees;
    
    if ( powerup == POWERUP_LASER ) {
        new_degrees *= 0.25f;
    }
    
    Entity::Rotate(new_degrees);
}


void Player::ResetPosition()
{
    position = player_info[id].position;
    velocity.Zero();
    SetOrientation(player_info[id].direction);
}



void Player::ShootBullet()
{
    if ( IsRespawning() || shoot_cooldown_timer > 0 || num_bullets == 0 ) {
        return;
    }
        
    int num_shots = powerup == POWERUP_MULTISHOT ? 3 : 1;
    Vec2 bullet_position = position + orientation * radius;
    
    for ( int i = 0; i < num_shots; i++ ) {
        Bullet * bullet = new Bullet(bullet_position, id);
        bullet->velocity = orientation;
        bullet->velocity *= BULLET_MAX_VELOCITY;
        bullet->velocity += velocity;
        if ( i == 1 ) {
            bullet->velocity.RotateByDegrees(-10);
        } else if ( i == 2 ) {
            bullet->velocity.RotateByDegrees(10);
        }
        
        game.entities.push_back(bullet);
    }
        
    shoot_cooldown_timer = SHOT_DELAY;
    num_bullets--;
    bullet_recharge_timer = BULLET_RECHARGE_TIME;
    
    if ( powerup == POWERUP_MULTISHOT ) {
        DOS_Play("t 180 o4 l32 c+ a > f > c+");
    } else {
        DOS_Play("t 180 o6 l32 c+ a");
    }
}


// dos_color = -1: random color
void Player::Explode(int dos_color)
{
    if ( !IsActive() ) {
        return;
    }
    
    respawn_timer.Start();
    shoot_cooldown_timer = 0;
    num_bullets = MAX_BULLETS;
    this->EmitParticles(100, dos_color);
    powerup = POWERUP_NONE;

    ResetPosition();
}


void Player::updateFromInputState(InputState input_state, float dt)
{
    float rotation_speed = PLAYER_ROTATION_SPEED * dt;
    
    if ( input_state.left )
        RotateByDegrees(-rotation_speed);
    
    if ( input_state.right )
        RotateByDegrees(+rotation_speed);
    
    if ( input_state.thrust )
        Thrust(dt);
    
    if ( input_state.shoot && (powerup != POWERUP_LASER) )
        ShootBullet();
    
    if ( input_state.shield ) {
        if ( shield_strength ) {
            shield_up = true;
        }
    } else {
        shield_up = false;
    }
}


void Player::Update(float dt)
{
    if ( num_lives == 0 ) {
        return;
    }

    if ( shield_up ) {
        --shield_strength;
        if ( shield_strength <= 0 ) {
            shield_up = false;
        }
    } else {
        if ( App::Shared()->GetTicks() % 3 == 0 ) {
            ++shield_strength;
            if ( shield_strength >= MAX_SHIELD_STRENGTH ) {
                shield_strength = MAX_SHIELD_STRENGTH;
            }
        }
    }
    
#if 0
    if ( shield_strength > 0 && shield_up ) {
        --shield_strength;
    } else if ( !shield_up ) {
        ++shield_strength;
        if ( shield_strength > MAX_SHIELD_STRENGTH ) {
            shield_strength = MAX_SHIELD_STRENGTH;
        }
    }
#endif
    
    // run timers
    
//    if ( respawn_timer > 0 ) {
//        --respawn_timer;
//        return;
//    }
    respawn_timer.Run();
    if ( !respawn_timer.Done() ) {
        return;
    }
    
    if ( shoot_cooldown_timer > 0 ) {
        --shoot_cooldown_timer;
    }
    
    if ( powerup_timer && --powerup_timer == 0 ) {
        powerup = POWERUP_NONE;
    }
    
    // respawn bullets
    
    if ( num_bullets < MAX_BULLETS ) {
        // run recharge timer
        if ( --bullet_recharge_timer <= 0 ) { // replenish bullet
            bullet_recharge_timer = BULLET_RECHARGE_TIME;
            num_bullets++;
        }
    }
    
    UpdatePosition(dt, true);
    
    // shoot laser if available
    
    if ( powerup == POWERUP_LASER ) {
        EmitParticles(1, RANDOM_COLORS);
        
        for ( int i = 0; i < game.GetNumPlayers(); i++ ) {
            if ( i == id || !game.players[i]->IsActive() ) {
                continue;
            }
            LaserPlayer(game.players[i]);
        }
    }
}



Vec2 Player::NozzlePoint()
{
    return position + orientation * radius;
}



Vec2 Player::LaserEndPoint()
{
    return position + orientation * GAME_W * 2;
}



void Player::LaserPlayer(Player * other_player)
{
    if ( LineCircleIntersection(NozzlePoint(),
                                LaserEndPoint(),
                                other_player->position,
                                other_player->radius) )
    {
        other_player->Explode(player_info[other_player->id].color);
        other_player->ExplosionSound();
    }
}



void Player::Draw(SDL_Renderer * renderer)
{
    if ( num_lives == 0 ) {
        return;
    }

#if DEBUG_VISUAL
    orientation *= 10.0f;
    DOS_SetColor(renderer->SDL(), DOS_BRIGHT_RED);
    DrawLine(renderer->SDL(), position, position + orientation);
    orientation.normalize();
    DOS_SetColor(renderer->SDL(), DOS_GRAY);
    DrawLine(renderer->SDL(), position, position + velocity);
    Entity::drawSprite(renderer, number);
#else
    // TODO: flash faster and faster
    //bool flash = respawn_timer < RESPAWN_TICKS / 2 && FlashInterval(100);
    bool flash = respawn_timer.GetTicks() < REAPPEAR_TICKS && FlashInterval(100);
    
    //if ( !respawn_timer || flash ) {
    if ( respawn_timer.Done() || flash ) {
        Entity::DrawSprite(renderer, id);
        switch ( powerup ) {
            case POWERUP_LASER: {
                DOS_SetColor(renderer, DOS_RandomColor());
                DrawLine(renderer, NozzlePoint(), LaserEndPoint());
                break;
            }
            case POWERUP_SHOWPATH: {
                DOS_Color color = player_info[id].color;
                DOS_SetColor(renderer, (DOS_Color)((int)color - 8));
                Vec2 pos = position;
                Vec2 vel = velocity;
                float dt = MS_PER_FRAME / 1000.0f;
                
                for ( int i = 0; i < 2000; i++ ) {
                    pos += vel * dt;
                    Vec2 v = Vec2(GAME_W/2, GAME_H/2) - pos;
                    v *= 1.0f / (v.Length() * BLACK_HOLE_GRAVITY) * dt;
                    vel += v;
                    if ( (i & 10) == 0 ) {
                        SDL_RenderDrawPoint(renderer, pos.x, pos.y);
                    }
                }
                break;
            }
            default:
                break;
        }
    }
#endif
    
    if ( shield_up ) {
        DOS_SetColor(renderer, DOS_RandomColor());
        DrawCircle(renderer, position.x, position.y, SHIELD_RADIUS);
        
        for ( int i = 0; i < 100; i++ ) {
            Vec2 pt = RandomPointWithinRange(SHIELD_RADIUS);
            DOS_SetColor(renderer, DOS_RandomColor());
            SDL_RenderDrawPoint(renderer, pt.x, pt.y);
        }
        
#if 0
        int diam = 18;
        int rad = diam / 2;
        SDL_Rect src = { number * diam, 0, diam, diam };
        SDL_Rect dst;
        dst.x = (int)position.x - rad;
        dst.y = (int)position.y - rad;
        dst.w = diam;
        dst.h = diam;
        
        ResourceManager * rm = &ResourceManager::shared();
        SDL_Texture * shield = rm->getTexture("shields.png", renderer);
        SDL_RenderCopy(renderer, shield, &src, &dst);
#endif
    }
}


void Player::SetPowerupEffect(Powerup * pup)
{
    this->powerup = pup->type;
    powerup_timer = pup->effect_time;
}


void Player::EatPowerup(Powerup * pup)
{
    switch ( pup->type ) {
        case POWERUP_ZEROG:
        case POWERUP_MULTISHOT:
        case POWERUP_LASER:
        case POWERUP_SHOWPATH:
            SetPowerupEffect(pup);
            break;
        case POWERUP_THREEUP:
            num_lives += 2; // fall-through
        case POWERUP_ONEUP:
            num_lives += 1;
            if ( num_lives > MAX_LIVES ) {
                num_lives = MAX_LIVES;
            }
            break;
        case POWERUP_EXTRABULLETS:
            num_bullets = 20;
            break;
        case POWERUP_NUKE: {
            // TODO: make sure this all works
            RandomizedSound(120, 100, 2000);

            for ( int i = 0; i < (int)game.entities.size(); i++ ) {
                // draw line
                switch ( game.entities[i]->type ) {
                    case ENTITY_PLAYER: {
                        Player * pl = (Player *)game.entities[i];
                        pl->Explode(DOS_RED);
                        pl->num_lives--;
                        break;
                    }
                    case ENTITY_BULLET:
                    case ENTITY_POWERUP:
                        game.entities[i]->EmitParticles(10, DOS_RED);
                        game.entities[i]->alive = false;
                        break;
                    default:
                        break;
                }
            }
            break;
        }
        default:
            break;
    }
    
    pup->alive = false;
}


void Player::Contact(Entity * hit)
{
    if ( !IsActive() ) {
        return;
    }
    
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * opponent = (Player *)hit;
            if ( opponent->IsActive() ) {
                num_lives--;
                opponent->num_lives--;
                Explode(RANDOM_COLORS);
                opponent->Explode(RANDOM_COLORS);
                ExplosionSound();
            }
            break;
        }
        case ENTITY_POWERUP:
            EatPowerup((Powerup *)hit);
            break;
        default:
            break;
    }
}



bool Player::IsDead()
{
    //return (respawn_timer > RESPAWN_TICKS / 2) || num_lives <= 0;
    return respawn_timer.GetTicks() > REAPPEAR_TICKS || num_lives <= 0;
}



bool Player::IsRespawning()
{
    //return respawn_timer > 0 && respawn_timer < RESPAWN_TICKS / 2;
    s16 ticks = respawn_timer.GetTicks();
    return ticks > 0 && ticks < REAPPEAR_TICKS;
}



bool Player::IsActive()
{
    return !IsRespawning() && !IsDead();
}



void Player::Thrust(float dt)
{
    if ( IsDead() ) {
        return;
    }
    
    if ( powerup == POWERUP_ZEROG ) {
        velocity += orientation * (PLAYER_THRUST + 50.0f) * dt;
    } else {
        velocity += orientation * PLAYER_THRUST * dt;
    }
    
    ShootExhaustParticles(100.0f, player_info[id].color);
}
