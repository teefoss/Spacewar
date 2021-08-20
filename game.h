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
    void Init();
    void Start(void);
    void Quit(void);
    int MaxPlayers(void);
    void SetNumPlayers(int n);
    int GetNumPlayers() { return num_players; }
    void Update(float dt);
    void NetUpdate(float dt);
    void ServerUpdate(float dt);
    void ClientUpdate();
    void Draw(SDL_Renderer * renderer);
    void GetPlayerInput(void);

    std::vector<Entity *> entities;
    Player * players[MAX_PLAYERS] = { NULL };
    bool paused = false;
    bool match_started = false;
    int num_players = 2;
    int powerups_on = true;
    int hazards_on = true;
    int black_hole_on = true;
    
private:
    void ClearEntities(void);
    void TrySpawnPowerup(void);
    void TrySpawnHazard(void);
    
    BlackHole * black_hole;
    Timer hazard_timer = Timer(SEC_TO_TICKS(20));
    int powerup_timer = SEC_TO_TICKS(10);
    InputState player_input[MAX_PLAYERS];
    int level = 1;
    
#if DEBUG_DATA
    DOS_Console * con;
#endif
};

extern Game game;
extern const Vec2 center;
extern const SDL_Rect visible_area;

#endif /* game_h */
