#include "Game.h"

#include <SDL.h>

int main(int, char**)
{
    Game game;
    if (!game.init("Connect 4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, 700, false)) {
        return 1;
    }

    // Cap the frame rate at 60 fps. Pieces fall a fixed distance per frame,
    // so this also keeps the animation speed the same on every machine.
    const Uint32 frameDuration = 1000 / 60;
    while (game.activated()) {
        const Uint32 frameStart = SDL_GetTicks();
        game.event();
        game.update();
        game.render();

        const Uint32 elapsed = SDL_GetTicks() - frameStart;
        if (elapsed < frameDuration) {
            SDL_Delay(frameDuration - elapsed);
        }
    }
    return 0;
}
