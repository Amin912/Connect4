#include "Game.h"

#include <SDL.h>

#include <algorithm>

int main(int, char**)
{
    Game game;
    if (!game.init("Four in a Row")) {
        return 1;
    }

    // Animations are driven by elapsed time, so they run at the same speed
    // whatever the frame rate. The frame cap only matters without vsync.
    const Uint64 frequency = SDL_GetPerformanceFrequency();
    Uint64 previous = SDL_GetPerformanceCounter();
    while (game.activated()) {
        const Uint64 now = SDL_GetPerformanceCounter();
        // Clamp long pauses (e.g. dragging the window) so pieces don't jump.
        const float dt = std::min(static_cast<float>(now - previous) / frequency, 0.05f);
        previous = now;

        game.event();
        game.update(dt);
        game.render();

        const Uint64 frameTicks = SDL_GetPerformanceCounter() - now;
        const Uint32 frameMs = static_cast<Uint32>(frameTicks * 1000 / frequency);
        if (frameMs < 8) {
            SDL_Delay(8 - frameMs);
        }
    }
    return 0;
}
