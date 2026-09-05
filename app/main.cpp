#include "Game.h"

#include <SDL.h>

#include <algorithm>

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

namespace {

Game* game = nullptr;
Uint64 previous = 0;

// One frame: input, update, draw. Animations are driven by elapsed time, so
// they run at the same speed whatever the frame rate.
void frame()
{
    const Uint64 now = SDL_GetPerformanceCounter();
    // Clamp long pauses (e.g. dragging the window) so pieces don't jump.
    const float dt = std::min(static_cast<float>(now - previous) / SDL_GetPerformanceFrequency(), 0.05f);
    previous = now;

    game->event();
    game->update(dt);
    game->render();
}

}  // namespace

int main(int, char**)
{
    static Game instance;
    if (!instance.init("Four in a Row")) {
        return 1;
    }
    game = &instance;
    previous = SDL_GetPerformanceCounter();

#ifdef __EMSCRIPTEN__
    // A browser page must not block, so the browser calls frame() once per
    // display refresh instead of the loop below.
    emscripten_set_main_loop(frame, 0, 1);
#else
    const Uint64 frequency = SDL_GetPerformanceFrequency();
    while (game->activated()) {
        const Uint64 start = SDL_GetPerformanceCounter();
        frame();
        // The frame cap only matters without vsync.
        const auto frameMs = static_cast<Uint32>((SDL_GetPerformanceCounter() - start) * 1000 / frequency);
        if (frameMs < 8) {
            SDL_Delay(8 - frameMs);
        }
    }
#endif
    return 0;
}
