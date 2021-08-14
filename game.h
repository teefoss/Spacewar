#ifndef game_h
#define game_h

#include "entity.h"
#include "defines.h"
#include "player.h"
#include "input.h"
#include "blackhole.h"

#include <vector>
#include <SDL2/SDL.h>

struct Menu;

class Game
{
public:
    Game() { };
    ~Game() { };
    void init();
    void start(void);
    void quit(void);
    int maxPlayers(void);
    void setNumPlayers(int n);
    int getNumPlayers() { return num_players; }
    void update(float dt);
    void draw(SDL_Renderer * renderer);
    void getPlayerInput(void);

    std::vector<Entity *> entities;
    Player * players[MAX_PLAYERS] = { NULL };
    bool paused = false;
    bool match_started = false;
    int num_players = 2;
    int aliens_on = true; // TODO: game options struct
    int powerups_on = true;
    int black_hole_moves = false;
    
private:
    void clearEntities(void);
    void trySpawnPowerup(void);
    
    BlackHole * black_hole;
    int powerup_timer = SEC_TO_TICKS(10);
    InputState player_input[MAX_PLAYERS];
    
#if DEBUG_DATA
    DOS_Console * con;
#endif
};

extern Game game;
extern const Vec2 center;

#endif /* game_h */
