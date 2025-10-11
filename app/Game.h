#pragma once

#include <SDL.h>

#include <memory>

class Grid;

// Owns the window and renderer and runs one frame at a time:
// event() -> update() -> render().
class Game
{
public:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Creates the window and loads the board. Returns false on failure,
    // after telling the user why.
    bool init(const char* title, int x, int y, int w, int h, bool fullscreen);

    void event();  // handles player input
    void update();
    void render();
    bool activated() const { return isActivated; }

    static SDL_Renderer* renderer;

private:
    SDL_Window* window = nullptr;
    bool isActivated = false;
    std::unique_ptr<Grid> grid;
};
