#pragma once

#include "Object.h"
#include "Texture.h"
#include "core/ai.h"
#include "core/board.h"

#include <SDL.h>

#include <memory>
#include <optional>
#include <string>

// The playing area: draws the board and pieces, turns mouse clicks into
// moves for the human player and lets the AI reply. The rules themselves
// live in connect4::Board.
class Grid
{
public:
    // Loads the images from assetDir. Throws std::runtime_error on failure.
    explicit Grid(const std::string& assetDir);

    void event(const SDL_Event& e);
    void update();
    void render() const;

private:
    enum class State { HumanTurn, AiTurn, Over };

    void play(int col, connect4::Player player);
    void restart();
    void setTitle(const char* title) const;

    int columnAt(int x) const;   // column under screen x, or -1
    int columnX(int col) const;  // screen x of a piece in that column
    int floorY(int row) const;   // y the bottom of a piece in that row rests on

    static constexpr connect4::Player Human = connect4::Player::One;
    static constexpr connect4::Player Ai = connect4::Player::Two;

    TexturePtr humanTexture;
    TexturePtr aiTexture;
    TexturePtr gridTexture;

    int gridX = 150;  // modify to change the grid position
    int gridY = 150;
    int checkSize = 80;

    connect4::Board board;
    connect4::ai::Options aiOptions;  // depth 6 with alpha-beta
    State state = State::HumanTurn;
    int aiDelay = 0;  // frames left before the AI moves
    std::optional<connect4::Line> winLine;

    std::unique_ptr<Object> gridShape;
    std::unique_ptr<Object> pieces[connect4::Board::Rows][connect4::Board::Cols];
};
