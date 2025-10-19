#pragma once

#include <SDL.h>

#include <memory>

class Grid;
class Menu;
class Text;
struct Sprites;

// Owns the window and renderer, switches between the menu and the board,
// and runs one frame at a time: event() -> update() -> render().
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
    enum class Screen { Menu, Playing };

    void drawBackground() const;

    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
    SDL_Cursor* arrowCursor = nullptr;
    SDL_Cursor* handCursor = nullptr;
    bool isActivated = false;
    Screen screen = Screen::Menu;

    std::unique_ptr<Sprites> sprites;
    std::unique_ptr<Text> text;
    std::unique_ptr<Menu> menu;
    std::unique_ptr<Grid> grid;
};
