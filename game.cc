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
#include "bullet.h"
#include "blackhole.h"
#include "player.h"

#include <stdlib.h>
#include <SDL2/SDL_net.h>

Game game = Game();
const Vec2 center = Vec2(GAME_W / 2, GAME_H / 2);

void Game::init()
{     
    // blackhole appears on title screen
    black_hole = new BlackHole();
    entities.push_back(black_hole);
    
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
}


void Game::clearEntities()
{
    for ( unsigned i = 0; i < entities.size(); i++ ) {
        delete entities[i];
    }
    
    entities.clear();
}


void Game::start()
{
    match_started = true;
    menu_is_open = false;

    clearEntities();
        
    black_hole = new BlackHole();
    entities.push_back(black_hole);
    
    for ( int i = 0; i < num_players; i++ ) {
        players[i] = new Player(i);
        entities.push_back(players[i]);
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

void Game::draw(SDL_Renderer * renderer)
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
    
    for ( unsigned i = 0; i < entities.size(); i++ ) {
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
        entities.push_back(powerup);
    }
}


void Game::update(float dt)
{
    black_hole->emitParticles(3, DOS_RED);
    particles.update(dt);

    if ( paused || menu_is_open ) {
        black_hole->update(dt); // the black hole always updates
        return;
    }

    trySpawnPowerup();
    
    for ( int i = 0; i < getNumPlayers(); i++ ) {
        players[i]->updateFromInputState(player_input[i], dt);
    }
    
    for ( unsigned i = 0; i <entities.size(); i++ ) {
        entities[i]->update(dt);
    }
            
    // resolve collisions
    
    int count = (int)entities.size();
    for ( int i = 0; i < count; i++ ) {
        for ( int j = i + 1; j < count; j++ ) {
            if ( entities[i]->isColliding(entities[j]) ) {
                entities[i]->contact(entities[j]);
                entities[j]->contact(entities[i]);
            }
        }
    }
    
    // remove dead stuff
    
    count = (int)entities.size();
    for ( int i = (int)count - 1; i >= 0; i-- ) {
        if ( !entities[i]->alive ) {
            delete entities[i];
            entities.erase(entities.begin() + i);
        }
    }
}


void Game::getPlayerInput()
{
    for ( int i = 0; i < game.getNumPlayers(); i++ ) {
        player_input[i] = input.getInputState(i);
    }
}


// TODO: App::run() instead, remove
#if 0
void Game::run()
{
    static float dt;
    int last_ms = 0;
    
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

#if 1
    LOG("size of player:  %zu bytes\n", sizeof(Player));
    LOG("size of bl hole: %zu bytes\n", sizeof(BlackHole));
    LOG("size of bullet:  %zu bytes\n", sizeof(Bullet));
    LOG("size of pup:     %zu bytes\n", sizeof(Powerup));
#endif
    
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
        
        // TODO: this is a total bollocking mess
        
        if ( is_network_game ) {
            if ( my_id == SERVER_ID ) {
                
                // get input from myself and clients
                input_states[0] = input.getInputState(0); // get my own state
                for ( client_id_t id = 0; id < num_clients; id++ ) {
                    input_states[id + 1] = HostReceiveInputState(id);
                }

                update(input_states, dt);
                
                // send updated game, for each client:
                for ( client_id_t id = 0; id < num_clients; id++ ) {
                    
                    // send  number of entities
                    u16 count = (u16)entities.size();
                    SDLNet_TCP_Send(clients[id], &count, sizeof(count));
                    
                    // for each entity
                    for ( u16 i = 0; i < count; i++ ) {
                        
                        // send type's size
                        u8 size = (u8)entities[i]->size();
                        SDLNet_TCP_Send(clients[id], &size, sizeof(size));
                        
                        // send entity
                        SDLNet_TCP_Send(clients[id], entities[i], size);
                    }
                }
            } else {
                InputState my_input = input.getInputState(0);
                ClientSendInputState(my_input);
                
                // get updated game from server
                clearEntities();
                
                // get num entities
                u16 num_entities;
                SDLNet_TCP_Recv(my_socket, &num_entities, sizeof(num_entities));
                
                // for each entity
                for ( u16 i = 0; i < num_entities; i++ ) {
                    
                    // get entity's type
                    u8 size;
                    SDLNet_TCP_Recv(my_socket, &size, sizeof(size));

                    // get entity
                    void * data = malloc(size);
                    SDLNet_TCP_Recv(my_socket, data, size);
                    
                    // reload texture and put it in
                    Entity * entity = (Entity *)data;
                    entity->loadTexture(entity->getTextureName());
                    entities.push_back(entity);
#if 0
                    switch ( (EntityType)type ) {
                        case ENTITY_BLACK_HOLE: {
                            BlackHole * data = (BlackHole *)malloc(sizeof(*data));
                            SDLNet_TCP_Recv(my_socket, data, sizeof(*data));
                            data->loadTexture(data->getTextureName());
                            entities.push_back((BlackHole *)data);
                            break;
                        }
                        case ENTITY_PLAYER: {
                            data = malloc(sizeof(Player));
                            SDLNet_TCP_Recv(my_socket, &data, sizeof(Player));
                            entities.push_back((Player *)data);
                            break;
                        }
                        case ENTITY_BULLET: {
                            data = malloc(sizeof(Bullet));
                            SDLNet_TCP_Recv(my_socket, &data, sizeof(Bullet));
                            entities.push_back((Bullet *)data);
                            break;
                        }
                        case ENTITY_POWERUP: {
                            data = malloc(sizeof(Powerup));
                            SDLNet_TCP_Recv(my_socket, &data, sizeof(Powerup));
                            entities.push_back((Powerup *)data);
                            break;
                        }
                        default:
                            break;
                    }
#endif
                }
            }
        } else {
            for ( int i = 0; i < num_players; i++ ) {
                input_states[i] = input.getInputState(i);
            }
            update(input_states, dt);
        }
        
        draw();
    }
    
    quit();
}
#endif
