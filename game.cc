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

void Game::Init()
{
    if ( is_network_game ) {
        num_players = num_clients + 1;
        printf("starting network game with %d players\n", num_players);
        match_started = true;
        menu_is_open = false;
        return;
    }
    
    // blackhole appears on title screen
    black_hole = new BlackHole();
    entities.push_back(black_hole);
    
#if DEBUG_DATA
    con = DOS_NewConsole(renderer->SDL(), GAME_W/2, GAME_H/8, DOS_MODE40);
    DOS_CSetBackground(con, DOS_TRANSPARENT);
    DOS_ClearBackground(con);
#endif
}


void Game::Quit()
{
    ClearEntities();
        
#if DEBUG_DATA
    DOS_FreeConsole(con);
#endif
}


void Game::ClearEntities()
{
    for ( unsigned i = 0; i < entities.size(); i++ ) {
        delete entities[i];
    }
    
    entities.clear();
}


void Game::Start()
{
    match_started = true;
    menu_is_open = false;

    ClearEntities();
        
    black_hole = new BlackHole();
    entities.push_back(black_hole);
    
    for ( int i = 0; i < num_players; i++ ) {
        players[i] = new Player(i);
        entities.push_back(players[i]);
    }
}


// max players allowed by current input status (how many controllers connected)
int Game::MaxPlayers()
{
    int max_players = KB_NUM_USERS + input.NumControllers();
    
    if ( max_players > MAX_PLAYERS ) {
        max_players = MAX_PLAYERS;
    }
    
    return max_players;
}

void Game::SetNumPlayers(int n)
{
    int number = n;
    
    CLAMP(number, 1, MaxPlayers());
    num_players = number;
}

void Game::Draw(SDL_Renderer * renderer)
{
    SDL_SetRenderDrawColor(renderer, 16, 16, 16, 255);
    SDL_RenderClear(renderer);

    DOS_SetColor(renderer, DOS_BLACK);
    SDL_RenderFillRect(renderer, NULL);
    
    star_field.Draw(renderer);
    
    if ( !menu_is_open && match_started ) {
        for ( int i = 0; i < num_players; i++ ) {
            players[i]->RenderHUD(renderer);
        }
    }

    particles.Draw(renderer);
    
    for ( unsigned i = 0; i < entities.size(); i++ ) {
        entities[i]->Draw(renderer);
    }
        
    if ( menu_is_open ) {
        DrawMenu();
        input.RenderConsole();
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


void Game::TrySpawnPowerup()
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


void Game::Update(float dt)
{
    black_hole->EmitParticles(3, DOS_RED);
    particles.Update(dt);

    if ( paused || menu_is_open ) {
        black_hole->Update(dt); // the black hole always updates
        return;
    }

    TrySpawnPowerup();
    
    for ( int i = 0; i < GetNumPlayers(); i++ ) {
        players[i]->updateFromInputState(player_input[i], dt);
    }
    
    for ( unsigned i = 0; i <entities.size(); i++ ) {
        entities[i]->Update(dt);
    }
            
    // resolve collisions
    
    int count = (int)entities.size();
    for ( int i = 0; i < count; i++ ) {
        for ( int j = i + 1; j < count; j++ ) {
            if ( entities[i]->IsColliding(entities[j]) ) {
                entities[i]->Contact(entities[j]);
                entities[j]->Contact(entities[i]);
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


void Game::GetPlayerInput()
{
    for ( int i = 0; i < game.GetNumPlayers(); i++ ) {
        player_input[i] = input.GetInputState(i);
    }
}


void Game::ServerUpdate(float dt)
{
    // get input from myself and clients
    player_input[0] = input.GetInputState(0); // get my own state
    for ( client_id_t id = 0; id < num_clients; id++ ) {
        player_input[id + 1] = HostReceiveInputState(id);
    }

    Update(dt);
    
    // send updated game to each client:
    for ( client_id_t id = 0; id < num_clients; id++ ) {
        
        // send number of entities
        u16 count = (u16)entities.size();
        SDLNet_TCP_Send(clients[id], &count, sizeof(count));
        
        // send each entity
        for ( u16 i = 0; i < count; i++ ) {
            // send type's size
            u8 size = (u8)entities[i]->Size();
            SDLNet_TCP_Send(clients[id], &size, sizeof(size));
            
            // send entity
            SDLNet_TCP_Send(clients[id], entities[i], size);
        }
    }
}


void Game::ClientUpdate()
{
    // send my input to the server
    InputState my_input = input.GetInputState(0);
    ClientSendInputState(my_input);
    
    ClearEntities();
    
    // get updated game from server:
    // receive num entities
    u16 num_entities;
    SDLNet_TCP_Recv(my_socket, &num_entities, sizeof(num_entities));
    
    // for each entity
    for ( u16 i = 0; i < num_entities; i++ ) {
        
        // receive entity's type
        u8 size;
        SDLNet_TCP_Recv(my_socket, &size, sizeof(size));

        // receive entity
        void * data = malloc(size);
        SDLNet_TCP_Recv(my_socket, data, size);
        
        // reload texture and put it in
        Entity * entity = (Entity *)data;
        entity->LoadTexture(entity->GetTextureName());
        entities.push_back(entity);
    }
}


void Game::NetUpdate(float dt)
{
    if ( my_id == SERVER_ID ) {
        ServerUpdate(dt);
    } else {
        ClientUpdate();
    }
}
