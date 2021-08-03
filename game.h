#ifndef game_h
#define game_h

#include "entity.h"
#include "defines.h"
#include "player.h"
#include "input.h"
#include "window.h"
#include "blackhole.h"

#include <vector>
#include <SDL2/SDL.h>

struct Menu;

class Game
{
public:
    Game() { };
    ~Game() { };
    void init(void);
    void run(void);
    void start(void);
    void quit(void);
    int maxPlayers(void);
    void setNumPlayers(int n);
    int numPlayers(void);
    
    Window window = Window(GAME_NAME, GAME_W*2, GAME_H*2, 0, 0);
    SDL_Renderer * renderer;
    Storage<Entity *> entities;
    Player * players[MAX_PLAYERS] = { NULL };
    bool running = true;
    bool paused = false;
    bool match_started = false;
    int ticks = 0;
    int num_players = 2;
    int aliens_on = true;
    int powerups_on = true;
    int black_hole_moves = false;
    
private:
    bool processEvent(SDL_Event * event);
    bool processKey(SDL_Keycode key);
    void processPlayerInput(float dt);
    void updateGame(InputState input_state[MAX_PLAYERS], float dt);
    void drawGame(void);
    void clearEntities(void);
    void trySpawnPowerup(void);
    
    BlackHole * black_hole;
    int powerup_timer = SEC_TO_TICKS(10);
    
#if DEBUG_DATA
    DOS_Console * con;
#endif
};

extern Game game;
extern const Vec2 center;

#endif /* game_h */
