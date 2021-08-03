#include "game.h"

#include "random.h"
#include "utility.h"
#include "menu.h"
#include "resources.h"
#include "particle.h"
#include "powerup.h"
#include "stars.h"
#include "log.h"
#include "net.h"

#include <stdlib.h>
#include <SDL2/SDL_net.h>

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
    clearEntities();
        
#if DEBUG_DATA
    DOS_FreeConsole(con);
#endif
    
    SDL_DestroyRenderer(renderer);
}


void Game::clearEntities()
{
    for ( unsigned i = 0; i < entities.count(); i++ ) {
        delete entities[i];
    }
    
    entities.empty();
}


void Game::start()
{
    match_started = true;
    menu_is_open = false;

    clearEntities();
        
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


void Game::trySpawnPowerup()
{
    if ( !powerups_on ) {
        return;
    }
    
    if ( --powerup_timer <= 0 ) {
        powerup_timer = Random(SEC_TO_TICKS(15), SEC_TO_TICKS(25));
        Powerup * powerup = new Powerup();
        entities.append(powerup);
    }
}


void Game::updateGame(InputState input_state[MAX_PLAYERS], float dt)
{
    black_hole->emitParticles(3, DOS_RED);
    particles.update(dt);

    if ( paused || menu_is_open ) {
        black_hole->update(dt); // the black hole always updates
        return;
    }

    trySpawnPowerup();
    
    for ( int i = 0; i < numPlayers(); i++ ) {
        players[i]->updateFromInputState(input_state[i], dt);
    }
    
    for ( unsigned i = 0; i <entities.count(); i++ ) {
        entities[i]->update(dt);
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
    
    // remove dead stuff
    
    count = entities.count();
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

//    LOG("size of entity: %zu bytes\n", sizeof(Entity));
//    LOG("size of player: %zu bytes\n", sizeof(Player));
//    LOG("size of entity data: %zu bytes\n", sizeof(EntityData));
    
    if ( is_network_game ) {
        num_players = num_clients + 1;
        printf("starting network game with %d players\n", num_players);
        
        if ( my_id != SERVER_ID ) {
            window.center();
        }
        
        // sync?
        if ( my_id == SERVER_ID ) {
            u8 check;
            for ( int i = 0; i < num_clients; i++ ) {
                SDLNet_TCP_Recv(my_socket, &check, sizeof(check));
            }
        } else {
            u8 ready = 1;
            SDLNet_TCP_Send(my_socket, &ready, sizeof(ready));
        }
        
        start();
    }
    
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
        
        InputState input_states[MAX_PLAYERS];
        
        if ( is_network_game ) {
            if ( my_id == SERVER_ID ) {
                // get input from myself and clients
                input_states[0] = input.getInputState(0); // get my own state
                for ( client_id_t id = 0; id < num_clients; id++ ) {
                    input_states[id + 1] = HostReceiveInputState(id);
                }

                updateGame(input_states, dt);
                
                // send updated game to clients
                unsigned count = entities.count();
                for ( int i = 0; i < num_clients; i++ ) {
                    SDLNet_TCP_Send(clients[i], &count, sizeof(count));
                    for ( unsigned j = 0; j < count; j++ ) {
                        SDLNet_TCP_Send(clients[i], entities[i], sizeof(*entities[i]));
                    }
                }
            } else {
                InputState my_input = input.getInputState(0);
                ClientSendInputState(my_input);
                clearEntities();
                unsigned count;
                SDLNet_TCP_Recv(my_socket, &count, sizeof(count));
                for ( unsigned i = 0; i < count; i++ ) {
                    SDLNet_TCP_Recv(my_socket, entities[i], sizeof(*entities[i]));
                }
            }
        } else {
            for ( int i = 0; i < num_players; i++ ) {
                input_states[i] = input.getInputState(i);
            }
            updateGame(input_states, dt);
        }
        
        drawGame();
    }
    
    quit();
}
