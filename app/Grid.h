#pragma once

#include "Button.h"
#include "Object.h"
#include "Paint.h"
#include "Settings.h"
#include "Text.h"
#include "core/ai.h"
#include "core/board.h"

#include <SDL.h>

#include <future>
#include <memory>
#include <optional>

// The game screen: draws the board and pieces, turns clicks and key presses
// into moves for the human player and lets the AI reply. The rules
// themselves live in connect4::Board.
class Grid
{
public:
    enum class Action { None, Menu };

    Grid(SDL_Renderer* renderer, const Sprites& sprites, Text& text);

    // Starts a new match with the options chosen on the menu. Resets the score.
    void start(const Settings& chosen);

    Action event(const SDL_Event& e);
    void update(float dt);
    // Returns true if the mouse is over something clickable.
    bool render() const;

private:
    enum class State { HumanTurn, AiTurn, Over };

    void play(int col, connect4::Player player);
    void startAiTurn();
    void dropSelected();
    void restart();  // next game: same settings, score kept

    bool drawHeader() const;
    void drawPreview() const;
    bool drawFooter() const;
    bool isWinningCell(int row, int col) const;
    int columnAt(float x, float y) const;  // column under a window position, or -1

    static constexpr connect4::Player Human = connect4::Player::One;
    static constexpr connect4::Player Ai = connect4::Player::Two;

    SDL_Renderer* renderer;
    const Sprites& sprites;
    Text& text;
    Button menuButton;
    Button playAgainButton;

    connect4::Board board;
    Settings settings;
    connect4::ai::Options aiOptions;
    State state = State::HumanTurn;
    float aiDelay = 0;        // minimum seconds left before the AI moves
    std::future<connect4::ai::Result> aiMove;  // the search, running on another thread
    float time = 0;           // seconds since start, for animations
    int selectedColumn = -1;  // where the preview piece is shown
    std::optional<connect4::Line> winLine;

    int humanWins = 0;
    int aiWins = 0;

    std::unique_ptr<Object> pieces[connect4::Board::Rows][connect4::Board::Cols];
};
