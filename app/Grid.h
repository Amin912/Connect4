#pragma once

#include "Object.h"
#include "Paint.h"
#include "core/ai.h"
#include "core/board.h"

#include <SDL.h>

#include <memory>
#include <optional>

// The playing area: draws the board and pieces, turns mouse clicks into
// moves for the human player and lets the AI reply. The rules themselves
// live in connect4::Board.
class Grid
{
public:
    Grid(SDL_Renderer* renderer, const Sprites& sprites);

    void event(const SDL_Event& e);
    void update(float dt);
    void render() const;

private:
    enum class State { HumanTurn, AiTurn, Over };

    void play(int col, connect4::Player player);
    void restart();
    void setTitle(const char* title) const;
    bool isWinningCell(int row, int col) const;

    int columnAt(float x) const;  // column under window x, or -1

    static constexpr connect4::Player Human = connect4::Player::One;
    static constexpr connect4::Player Ai = connect4::Player::Two;

    SDL_Renderer* renderer;
    const Sprites& sprites;

    connect4::Board board;
    connect4::ai::Options aiOptions;  // depth 6 with alpha-beta
    State state = State::HumanTurn;
    float aiDelay = 0;  // seconds left before the AI moves
    float time = 0;     // seconds since the screen opened, for animations
    std::optional<connect4::Line> winLine;

    std::unique_ptr<Object> pieces[connect4::Board::Rows][connect4::Board::Cols];
};
