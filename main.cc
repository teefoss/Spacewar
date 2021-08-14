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

#include "app.h"

int main(int argc, char ** argv)
{
    App * app = new App(argc, argv);
    app->Run();
    delete app;
    
    return 0;
}
