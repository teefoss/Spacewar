#include "player.h"

#include "random.h"
#include "utility.h"
#include "resources.h"
#include "bullet.h"
#include "particle.h"
#include "game.h"

#include <dos.h>

typedef struct
{
    Vec2        position;
    Cardinal    direction;
    DOS_Color   color;
} PlayerInfo;

const int player_right = GAME_W - PLAYER_MARGIN;
const int player_bottom = GAME_H - PLAYER_MARGIN;

const PlayerInfo player_info[MAX_PLAYERS] =
{
    // PLAYER 1 [0] UPPER LEFT
    {
        .position = Vec2(PLAYER_MARGIN, PLAYER_MARGIN),
        .direction = CARDINAL_SOUTHEAST,
        .color = DOS_BRIGHT_GREEN,
    },
    // PLAYER 1 [1] LOWER RIGHT
    {
        .position = Vec2(player_right, player_bottom),
        .direction = CARDINAL_NORTHWEST,
        .color = DOS_YELLOW,
    },
    // PLAYER 3 [2] LOWER LEFT
    {
        .position = Vec2(PLAYER_MARGIN, player_bottom),
        .direction = CARDINAL_NORTHEAST,
        .color = DOS_BRIGHT_CYAN,
    },
    // PLAYER 4 [3] UPPER RIGHT
    {
        .position = Vec2(player_right, PLAYER_MARGIN),
        .direction = CARDINAL_SOUTHWEST,
        .color = DOS_BRIGHT_MAGENTA,
    },
};



Player::Player(int index)
: Entity(ENTITY_PLAYER, Vec2(0, 0), PLAYER_RADIUS, "ships.png")
{
    const PlayerInfo * info = &player_info[index];
    color = info->color;
    initial_position = info->position;
    initial_direction = info->direction;
    resetPosition();

    number = index;
    respawn_timer = 0;
    shoot_cooldown_timer = 0;
    powerup_timer = 0;
    powerup = POWERUP_NONE;
    num_bullets = MAX_BULLETS;
    num_lives = MAX_LIVES;
    shield_up = false;
    shield_strength = MAX_SHIELD_STRENGTH;
    
    // init HUD
    
    int diam = (int)(radius * 2.0f);
    hud_rect.w = MAX_LIVES * diam + MAX_LIVES * 2;
    hud_rect.h = diam + 10;
    
    if ( number == 0 || number == 2 ) {
        hud_rect.x = HUD_MARGIN;
    } else {
        hud_rect.x = GAME_W - HUD_MARGIN - hud_rect.w;
    }
    
    if ( number == 0 || number == 3 ) {
        hud_rect.y = HUD_MARGIN;
    } else {
        hud_rect.y = GAME_H - HUD_MARGIN - hud_rect.h;
    }
    
    hud_texture = SDL_CreateTexture(game.renderer,
                                    SDL_PIXELFORMAT_RGBA8888,
                                    SDL_TEXTUREACCESS_TARGET,
                                    hud_rect.w,
                                    hud_rect.h);
    if ( hud_texture == NULL ) {
        fprintf(stderr, "could not create ship HUD texture\n");
        exit(EXIT_FAILURE);
    }
    
    SDL_SetTextureBlendMode(hud_texture, SDL_BLENDMODE_BLEND);
}



Player::~Player()
{
    SDL_DestroyTexture(hud_texture);
}


Data Player::data()
{
    Data d;
    d.buffer = malloc(sizeof(PlayerData));
    d.size = sizeof(PlayerData);

    PlayerData * buf = (PlayerData *)d.buffer;
    buf->number = number;
    buf->num_lives = num_lives;
    buf->num_bullets = num_bullets;
    buf->powerup = powerup;
    buf->shield_strength = shield_strength;
    buf->shield_up = shield_up;
    buf->respawn_timer = respawn_timer;
    buf->shoot_cooldown_timer = shoot_cooldown_timer;
    buf->bullet_recharge_timer = bullet_recharge_timer;
    buf->powerup_timer = powerup_timer;
    
    return d;
}


void Player::makeHUDTexture(SDL_Renderer * renderer)
{
    // TODO: class members?
    ResourceManager& rm = ResourceManager::shared();
    SDL_Texture * ship_texture = rm.getTexture("ships.png", renderer);
    SDL_Texture * bullet_texture = rm.getTexture("bullets.png", renderer);
    
    SDL_SetRenderTarget(renderer, hud_texture);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
    
    // lives
    
    const int margin = 2;
    const int ship_diam = (int)(radius * 2.0f);
    SDL_Rect src = { number * ship_diam, 0, ship_diam, ship_diam };
    SDL_Rect dst = { 0, 0, ship_diam, ship_diam };
    
    for ( int i = 0; i < num_lives; i++ ) {
        SDL_RenderCopyEx(renderer, ship_texture, &src, &dst, 270, NULL, SDL_FLIP_NONE);
        dst.x += ship_diam + margin;
    }
    
    // shield strength
        
    SDL_Rect bar[2];
    for ( int i = 0; i < 2; i++ ) {
        bar[i].x = 0;
        bar[i].y = ship_diam + margin;
        bar[i].w = hud_rect.w - margin;
        bar[i].h = 3;
    }
    bar[1].w *= (float)shield_strength / (float)MAX_SHIELD_STRENGTH;

    DOS_SetColor(renderer, DOS_GRAY);
    SDL_RenderFillRect(renderer, &bar[0]);
    DOS_SetColor(renderer, color);
    SDL_RenderFillRect(renderer, &bar[1]);
    
    // bullets
    
    const int bullet_diam = (int)(BULLET_RADIUS * 2.0f);
    src = (SDL_Rect){ number * bullet_diam, 0, bullet_diam, bullet_diam };
    dst = (SDL_Rect){ 0, bar[0].y + bar[0].h + margin, bullet_diam, bullet_diam };
    
    DOS_SetColor(renderer, color);
    for ( int i = 0; i < num_bullets; i++ ) {
        SDL_RenderCopy(renderer, bullet_texture, &src, &dst);
        dst.x += bullet_diam + margin;
    }
    
    SDL_SetRenderTarget(renderer, NULL);
    rm.destroyTexture("ships.png");
    rm.destroyTexture("bullets.png");
}



void Player::renderHUD(SDL_Renderer * renderer)
{
    makeHUDTexture(renderer); // TODO: only as needed
    
    SDL_RendererFlip flip;
    
    if ( number == 1 || number == 3 ) {
        flip = SDL_FLIP_HORIZONTAL;
    } else {
        flip = SDL_FLIP_NONE;
    }

    SDL_RenderCopyEx(renderer, hud_texture, NULL, &hud_rect, 0, NULL, flip);
}


void Player::explosionSound()
{
    RandomizedSound(60, 400, 1600);
}


void Player::rotateByDegrees(float degrees)
{
    float new_degrees = degrees;
    
    if ( powerup == POWERUP_LASER ) {
        new_degrees *= 0.25f;
    }
    
    Entity::rotateByDegrees(new_degrees);
}


void Player::resetPosition()
{
    position = initial_position;
    velocity.zero();
    setOrientation(initial_direction);
}



void Player::shootBullet()
{
    if ( isRespawning() ) {
        return;
    }
    
    if ( shoot_cooldown_timer > 0 || !num_bullets ) {
        return;
    }
    
    int num_shots = powerup == POWERUP_MULTISHOT ? 3 : 1;
    Vec2 bullet_position = position + orientation * radius;
    
    for ( int i = 0; i < num_shots; i++ ) {
        Bullet * bullet = new Bullet(bullet_position, number);
        bullet->velocity = orientation;
        bullet->velocity *= BULLET_MAX_VELOCITY;
        bullet->velocity += velocity;
        if ( i == 1 ) {
            bullet->velocity.rotateByDegrees(-10);
        } else if ( i == 2 ) {
            bullet->velocity.rotateByDegrees(10);
        }
        
        game.entities.append(bullet);
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
void Player::explode(int dos_color)
{
    if ( !isActive() ) {
        return;
    }
    
    respawn_timer = RESPAWN_TICKS;
    shoot_cooldown_timer = 0;
    num_bullets = MAX_BULLETS;
    this->emitParticles(100, dos_color);
    powerup = POWERUP_NONE;

    resetPosition();
}


void Player::updateFromInputState(InputState input_state, float dt)
{
    float rotation_speed = PLAYER_ROTATION_SPEED * dt;
    
    if ( input_state.left )
        rotateByDegrees(-rotation_speed);
    
    if ( input_state.right )
        rotateByDegrees(+rotation_speed);
    
    if ( input_state.thrust )
        thrust(dt);
    
    if ( input_state.shoot && (powerup != POWERUP_LASER) )
        shootBullet();
    
    if ( input_state.shield ) {
        if ( shield_strength ) {
            shield_up = true;
        }
    } else {
        shield_up = false;
    }
}


void Player::update(float dt)
{
    if ( num_lives == 0 ) {
        return;
    }

    if ( shield_strength > 0 && shield_up ) {
        --shield_strength;
    } else if ( !shield_up ) {
        ++shield_strength;
        if ( shield_strength > MAX_SHIELD_STRENGTH ) {
            shield_strength = MAX_SHIELD_STRENGTH;
        }
    }
    
    // run timers
    
    if ( respawn_timer > 0 ) {
        --respawn_timer;
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
    
    updatePosition(dt, true);
    
    // shoot laser if available
    
    if ( powerup == POWERUP_LASER ) {
        emitParticles(1, RANDOM_COLORS);
        
        for ( int i = 0; i < game.numPlayers(); i++ ) {
            if ( i == number || !game.players[i]->isActive() ) {
                continue;
            }
            laserPlayer(game.players[i]);
        }
    }
}



Vec2 Player::nozzlePoint()
{
    return position + orientation * radius;
}



Vec2 Player::laserEndPoint()
{
    return position + orientation * GAME_W * 2;
}



void Player::laserPlayer(Player * other_player)
{
    if ( LineCircleIntersection(nozzlePoint(),
                                laserEndPoint(),
                                other_player->position,
                                other_player->radius) )
    {
        other_player->explode(other_player->color);
        other_player->explosionSound();
    }
}



void Player::draw(SDL_Renderer * renderer)
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
    bool flash = respawn_timer < RESPAWN_TICKS / 2 && FlashInterval(100);
    
    if ( !respawn_timer || flash ) {
        Entity::drawSprite(renderer, number);
        switch ( powerup ) {
            case POWERUP_LASER: {
                DOS_SetColor(renderer, DOS_RandomColor());
                DrawLine(renderer, nozzlePoint(), laserEndPoint());
                break;
            }
            case POWERUP_SHOWPATH: {
                DOS_SetColor(renderer, (DOS_Color)((int)color - 8));
                Vec2 pos = position;
                Vec2 vel = velocity;
                float dt = MS_PER_FRAME / 1000.0f;
                
                for ( int i = 0; i < 2000; i++ ) {
                    pos += vel * dt;
                    Vec2 v = Vec2(GAME_W/2, GAME_H/2) - pos;
                    v *= 1.0f / (v.length() * BLACK_HOLE_GRAVITY) * dt;
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
            Vec2 pt = randomPointWithinRange(SHIELD_RADIUS);
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


void Player::setPowerupEffect(Powerup * pup)
{
    this->powerup = pup->type;
    powerup_timer = pup->effect_time;
}


void Player::eatPowerup(Powerup * pup)
{
    switch ( pup->type ) {
        case POWERUP_ZEROG:
        case POWERUP_MULTISHOT:
        case POWERUP_LASER:
        case POWERUP_SHOWPATH:
            setPowerupEffect(pup);
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

            for ( unsigned i = 0; i < game.entities.count(); i++ ) {
                // draw line
                switch ( game.entities[i]->type ) {
                    case ENTITY_PLAYER: {
                        Player * pl = (Player *)game.entities[i];
                        pl->explode(DOS_RED);
                        pl->num_lives--;
                        break;
                    }
                    case ENTITY_BULLET:
                    case ENTITY_POWERUP:
                        game.entities[i]->emitParticles(10, DOS_RED);
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


void Player::contact(Entity * hit)
{
    if ( !isActive() ) {
        return;
    }
    
    switch ( hit->type ) {
        case ENTITY_PLAYER: {
            Player * opponent = (Player *)hit;
            if ( opponent->isActive() ) {
                num_lives--;
                opponent->num_lives--;
                explode(RANDOM_COLORS);
                opponent->explode(RANDOM_COLORS);
                explosionSound();
            }
            break;
        }
        case ENTITY_POWERUP:
            eatPowerup((Powerup *)hit);
            break;
        default:
            break;
    }
}



bool Player::isDead()
{
    return (respawn_timer > RESPAWN_TICKS / 2) || num_lives <= 0;
}



bool Player::isRespawning()
{
    return respawn_timer > 0 && respawn_timer < RESPAWN_TICKS / 2;
}



bool Player::isActive()
{
    return !isRespawning() && !isDead();
}



void Player::thrust(float dt)
{
    if ( powerup == POWERUP_ZEROG ) {
        velocity += orientation * (PLAYER_THRUST + 50.0f) * dt;
    } else {
        velocity += orientation * PLAYER_THRUST * dt;
    }
    
    // exhaust
    Vec2 origin, exhaust_v;
    
    origin = randomPointWithinRange(radius / 2.0f);
    exhaust_v = orientation;
    exhaust_v.normalize();
    exhaust_v.rotateByDegrees(180);
    exhaust_v *= 100.0f;
    SDL_Color sdl_color = DOS_CGAColorToSDLColor(color);
    int time = Random(0, 50);
    particles.spawn(origin, exhaust_v, time, sdl_color);
}
