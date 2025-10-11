#include "Game.h"

#include "Grid.h"

#include <exception>
#include <string>

SDL_Renderer* Game::renderer = nullptr;

namespace {

// Images are looked up next to the executable, not in the current working
// directory, so the game also starts when launched from somewhere else.
std::string assetDirectory()
{
    std::string dir = "assets/";
    if (char* base = SDL_GetBasePath()) {
        dir = std::string(base) + dir;
        SDL_free(base);
    }
    return dir;
}

void fail(const std::string& message)
{
    SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "%s", message.c_str());
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "Connect 4", message.c_str(), nullptr);
}

}  // namespace

Game::Game() = default;

Game::~Game()
{
    // Textures belong to the renderer, so the grid goes first.
    grid.reset();
    if (renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }
    if (window) {
        SDL_DestroyWindow(window);
    }
    SDL_Quit();
}

bool Game::init(const char* title, int x, int y, int w, int h, bool fullscreen)
{
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        fail(std::string("Could not start SDL: ") + SDL_GetError());
        return false;
    }

    window = SDL_CreateWindow(title, x, y, w, h, fullscreen ? SDL_WINDOW_FULLSCREEN : 0);
    if (!window) {
        fail(std::string("Could not create the window: ") + SDL_GetError());
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        fail(std::string("Could not create the renderer: ") + SDL_GetError());
        return false;
    }
    SDL_SetRenderDrawColor(renderer, 255, 215, 100, 255);

    try {
        grid = std::make_unique<Grid>(assetDirectory());
    } catch (const std::exception& e) {
        fail(e.what());
        return false;
    }

    isActivated = true;
    return true;
}

void Game::event()
{
    // Handle every event that arrived since the last frame.
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            isActivated = false;
        } else {
            grid->event(event);
        }
    }
}

void Game::update()
{
    grid->update();
}

void Game::render()
{
    SDL_RenderClear(renderer);
    grid->render();
    SDL_RenderPresent(renderer);
}
