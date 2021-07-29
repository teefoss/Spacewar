//   ---------------------------------------------------------------------------
//   Spacewar!
//   version by Thomas Foster (foster.pianist@gmail.com)
//   July 2021
//   ---------------------------------------------------------------------------
//   CREDITS
//   Spacewar! was conceived in 1961 by Martin Graetz, Stephen Russell, and
//   Wayne Wiitanen. It was first realized on the PDP-1 in 1962 by Stephen
//   Russell, Peter Samson, Dan Edwards, and Martin Graetz, together with
//   Alan Kotok, Steve Piner, and Robert A Saunders.
//   ---------------------------------------------------------------------------
//   NOTES / IDEAS
// - warp (+1 if kill, max 1)
// - powerup: mega shot
// - powerup: homing shot
// - powerup: amplify gravity for other players
// - 1-player game: enemies with AI
// - option: no powerups
//   ---------------------------------------------------------------------------
//   TODO
// - shield
// - online match?
// - black hole moves
//   ---------------------------------------------------------------------------
//   Resources
// - SDL_Net class
//   http://www.sdltutorials.com/sdl-net-part-1-user-tutorial
// - Game Object Storage               https://gamedev.stackexchange.com/questions/33888/what-is-the-most-efficient-container-to-store-dynamic-game-objects-in

#include "game.h"
#include "storage.h"
#include "log.h"
#include "types.h"
#include "net.h"

#include <SDL2/SDL.h>

#include <stdlib.h>
#include <stdint.h>

int main(int argc, char ** argv)
{
    if ( !Log::create("log.txt") ) {
        fprintf(stderr, "failed to created log file\n");
        return EXIT_FAILURE;
    }
        
    u32 flags = 0;
    flags |= SDL_INIT_VIDEO;
    flags |= SDL_INIT_AUDIO;
    flags |= SDL_INIT_GAMECONTROLLER;
    
    if ( SDL_Init(flags) < 0 ) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }
        
    atexit(SDL_Quit);
    
    NetInit(argc, argv);
    
    game.init();
    game.run();
    
    NetQuit();
    
    Log::destroy();
    
    return EXIT_SUCCESS;
}
