#pragma once

#include <SDL.h>

#include <memory>

class Grid;
struct Sprites;

// Owns the window and renderer and runs one frame at a time:
// event() -> update() -> render().
class Game
{
public:
    Game();
    ~Game();
    Game(const Game&) = delete;
    Game& operator=(const Game&) = delete;

    // Creates the window and everything drawn in it. Returns false on
    // failure, after telling the user why.
    bool init(const char* title);

    void event();  // handles player input
    void update(float dt);
    void render();
    bool activated() const { return isActivated; }

private:
    void drawBackground() const;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    bool isActivated = false;
    std::unique_ptr<Sprites> sprites;
    std::unique_ptr<Grid> grid;
};
