#include "game.h"

#include "random.h"
#include "utility.h"
#include "menu.h"
#include "resources.h"
#include "particle.h"
#include "powerup.h"
#include "stars.h"
#include "log.h"

#include <stdlib.h>

Game game = Game();
const Vec2 center = Vec2(GAME_W / 2, GAME_H / 2);

void Game::init()
{     
    renderer = SDL_CreateRenderer(window.SDL(), -1, 0);
    if ( renderer == NULL ) {
        exit(EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(renderer, GAME_W, GAME_H);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    InitMenu();
    input.init(renderer);
    DOS_InitSound();

    // blackhole appears on title screen
    black_hole = new BlackHole();
    entities.append(black_hole);
    
#if DEBUG_DATA
    con = DOS_NewConsole(renderer->SDL(), GAME_W/2, GAME_H/8, DOS_MODE40);
    DOS_CSetBackground(con, DOS_TRANSPARENT);
    DOS_ClearBackground(con);
#endif
}



void Game::quit()
{
    for ( unsigned i = 0; i < entities.count(); i++ ) {
        delete entities[i];
    }
        
#if DEBUG_DATA
    DOS_FreeConsole(con);
#endif
    
    SDL_DestroyRenderer(renderer);
}



void Game::start()
{
    match_started = true;
    menu_is_open = false;

    Entity * e = entities[0];
    for ( unsigned i = 0; i < entities.count(); i++, e++ ) {
        delete e;
    }
    entities.empty();
        
    black_hole = new BlackHole();
    entities.append(black_hole);
    
    for ( int i = 0; i < num_players; i++ ) {
        players[i] = new Player(i);
        entities.append(players[i]);
    }
}


// max players allowed by current input status (how many controllers connected)
int Game::maxPlayers()
{
    int max_players = KB_NUM_USERS + input.numControllers();
    
    if ( max_players > MAX_PLAYERS ) {
        max_players = MAX_PLAYERS;
    }
    
    return max_players;
}

void Game::setNumPlayers(int n)
{
    int number = n;
    
    CLAMP(number, 1, maxPlayers());
    num_players = number;
}


int Game::numPlayers()
{
    return num_players;
}


bool Game::processKey(SDL_Keycode key)
{
    switch ( key ) {
        case SDLK_ESCAPE:
            menu_is_open = !menu_is_open;
            DOS_QueueSound(1320, 50);
            DOS_QueueSound(1100, 50);
            DOS_PlayQueuedSound();
            break;
        case SDLK_BACKSLASH:
            window.toggleFullscreen();
            break;
        case SDLK_p:
            paused = !paused;
        default:
            break;
    }
    
    return true;
}



bool Game::processEvent(SDL_Event * event)
{
    switch ( event->type ) {
            
        case SDL_QUIT:
            return false;
            
        case SDL_KEYDOWN:
            if ( menu_is_open ) {
                return ProcessMenuKey(event->key.keysym.sym);
            } else {
                return processKey(event->key.keysym.sym);
            }
            break;
            
        case SDL_CONTROLLERDEVICEADDED:
        case SDL_CONTROLLERDEVICEREMOVED:
            input.reconnectControllers();
            break;
            
        case SDL_CONTROLLERBUTTONDOWN:
            if ( event->cbutton.button == SDL_CONTROLLER_BUTTON_START ) {
                menu_is_open = !menu_is_open;
            }
            
            if ( menu_is_open ) {
                return ProcessMenuControllerButton(event->cbutton.button);
            }
            break;
            
        default:
            break;
    }
    
    return true;

}



void Game::drawGame()
{
    SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
    SDL_RenderClear(renderer);

    DOS_SetColor(renderer, DOS_BLACK);
    SDL_RenderFillRect(renderer, NULL);
    
    star_field.draw(renderer);
    
    if ( !menu_is_open && match_started ) {
        for ( int i = 0; i < num_players; i++ ) {
            players[i]->renderHUD(renderer);
        }
    }

    particles.draw(renderer);
    
    
    for ( unsigned i = 0; i < entities.count(); i++ ) {
        entities[i]->draw(renderer);
    }
        
    if ( menu_is_open ) {
        DrawMenu();
        input.renderConsole();
    }
    
#if DEBUG_DATA
    if ( match_started ) {
        DOS_CGotoXY(con, 0, 0);
        DOS_CPrintString(con, "Player 1 angle: %f\n", players[0]->angle);
        DOS_CPrintString(con, "Player 2 angle: %f\n", players[1]->angle);
        DOS_RenderConsole(con, 0, 0);
    }
#endif
    
    SDL_RenderPresent(renderer);
}


void Game::updateGame(float dt)
{
    black_hole->emitParticles(3, DOS_RED);
    particles.update(dt);

    if ( paused || menu_is_open ) {
        black_hole->update(dt);
        return;
    }
    
    // TODO: refactor: TrySpawnPowerup
    if ( powerups_on ) {
        if ( ticks > next_powerup_tick ) {
            next_powerup_tick = ticks + Random(SEC_TO_TICKS(15), SEC_TO_TICKS(25));
            Powerup * powerup = new Powerup();
            entities.append(powerup);
        }
    }
    
    for ( unsigned i = 0; i <entities.count(); i++ ) {
        entities[i]->update(dt);
    }
    
    // TODO: move to BlackHole update
    // black hole gravity
    for ( int i = 0; i < num_players; i++ ) {
        if (players[i]
            && players[i]->isActive()
            && players[i]->powerup != POWERUP_ZEROG )
        {
            black_hole->exertGravity(players[i], BLACK_HOLE_GRAVITY, dt);
        }
    }
    
    // TODO: move to player->update
    // player lasers
    for ( int i = 0; i < num_players; i++ ) {
        if ( !players[i]->isActive() ) {
            continue;
        }
        
        if ( players[i]->powerup == POWERUP_LASER ) {
            players[i]->emitParticles(1, RANDOM_COLORS);
            for ( int j = 0; j < num_players; j++ ) {
                if ( i == j || !players[j]->isActive() ) {
                    continue;
                }
                players[i]->laserPlayer(players[j]);
            }
        }
    }
    
    // resolve collisions
    int count = entities.count();
    for ( int i = 0; i < count; i++ ) {
        for ( int j = i + 1; j < count; j++ ) {
            if ( entities[i]->isColliding(entities[j]) ) {
                entities[i]->contact(entities[j]);
                entities[j]->contact(entities[i]);
            }
        }
    }
    
    // removed dead entities
    for ( int i = count - 1; i >= 0; i-- ) {
        if ( !entities[i]->alive ) {
            delete entities[i];
            entities.remove(i);
        }
    }
}



void Game::run()
{
    static float dt;
    int last_ms = 0;

    while ( running ) {
        ticks++;
        
        // limit framerate
        int current_ms, elapsed_ms;
        do {
            current_ms = SDL_GetTicks();
            elapsed_ms = current_ms - last_ms;
            if ( elapsed_ms > MS_PER_FRAME ) {
                break;
            }
            SDL_Delay(1);
        } while ( elapsed_ms < MS_PER_FRAME );
        dt = ((float)current_ms - (float)last_ms) / 1000.0f;
        last_ms = current_ms;
        
        SDL_Event event;
        while ( SDL_PollEvent(&event) ) {
            running = processEvent(&event);
        }
                
        updateGame(dt);
        drawGame();
    }
    
    quit();
}
