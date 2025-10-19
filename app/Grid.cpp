#include "Grid.h"

#include "Theme.h"

#include <cmath>

using connect4::Board;
using connect4::Player;

namespace {

// Pause before the AI replies, so the player's piece lands first.
constexpr float AiDelaySeconds = 0.55f;

constexpr const char* DefaultTitle = "Four in a Row";

}  // namespace

Grid::Grid(SDL_Renderer* renderer, const Sprites& sprites) : renderer(renderer), sprites(sprites)
{
}

int Grid::columnAt(float x) const
{
    const float offset = x - theme::BoardX - theme::BoardPadding;
    if (offset < 0) {
        return -1;
    }
    const int col = static_cast<int>(offset / theme::Cell);
    return col < Board::Cols ? col : -1;
}

void Grid::event(const SDL_Event& e)
{
    if (e.type != SDL_MOUSEBUTTONDOWN || e.button.button != SDL_BUTTON_LEFT) {
        return;
    }
    if (state == State::Over) {
        restart();
        return;
    }
    if (state == State::HumanTurn) {
        const int col = columnAt(static_cast<float>(e.button.x));
        if (board.canPlay(col)) {
            play(col, Human);
        }
    }
}

void Grid::play(int col, Player player)
{
    const int row = board.play(col, player);
    const bool human = player == Human;
    pieces[row][col] = std::make_unique<Object>(
        theme::columnCenterX(col), theme::DropZoneY,
        human ? sprites.humanPiece.get() : sprites.aiPiece.get());

    if (board.connectsFour(row, col)) {
        state = State::Over;
        winLine = board.findFour();
        setTitle(human ? "You win! Click to play again."
                       : "The AI wins. Click to play again.");
    } else if (board.isFull()) {
        state = State::Over;
        setTitle("Draw. Click to play again.");
    } else if (human) {
        state = State::AiTurn;
        aiDelay = AiDelaySeconds;
    } else {
        state = State::HumanTurn;
    }
}

void Grid::start(const Settings& chosen)
{
    settings = chosen;
    aiOptions.depth = searchDepth(settings.difficulty);
    restart();
}

void Grid::restart()
{
    board = Board{};
    for (auto& row : pieces) {
        for (auto& piece : row) {
            piece.reset();
        }
    }
    winLine.reset();
    if (settings.humanFirst) {
        state = State::HumanTurn;
    } else {
        state = State::AiTurn;
        aiDelay = AiDelaySeconds;
    }
    setTitle(DefaultTitle);
}

void Grid::setTitle(const char* title) const
{
    SDL_SetWindowTitle(SDL_RenderGetWindow(renderer), title);
}

bool Grid::isWinningCell(int row, int col) const
{
    if (!winLine) {
        return false;
    }
    for (const auto& cell : winLine->cells) {
        if (cell.row == row && cell.col == col) {
            return true;
        }
    }
    return false;
}

void Grid::update(float dt)
{
    time += dt;
    for (int row = 0; row < Board::Rows; ++row) {
        for (int col = 0; col < Board::Cols; ++col) {
            if (pieces[row][col]) {
                pieces[row][col]->fall(theme::rowCenterY(row), dt);
            }
        }
    }

    if (state == State::AiTurn) {
        aiDelay -= dt;
        if (aiDelay <= 0) {
            play(connect4::ai::bestMove(board, Ai, aiOptions).column, Ai);
        }
    }
}

void Grid::render() const
{
    const float scale = sprites.scale;
    Texture::draw(renderer, sprites.boardShadow.get(), theme::BoardX - Sprites::ShadowBlur,
                  theme::BoardY - Sprites::ShadowBlur + 14, scale);
    Texture::draw(renderer, sprites.boardBack.get(), theme::BoardX, theme::BoardY, scale);

    // Pieces sit between the back panel and the front of the board, so they
    // show through the holes. Once the game is won, the others fade back.
    for (int row = 0; row < Board::Rows; ++row) {
        for (int col = 0; col < Board::Cols; ++col) {
            if (pieces[row][col]) {
                const bool dim = winLine && !isWinningCell(row, col);
                pieces[row][col]->render(renderer, scale, dim ? 90 : 255);
            }
        }
    }
    Texture::draw(renderer, sprites.boardFront.get(), theme::BoardX, theme::BoardY, scale);

    if (winLine) {
        const float pulse = 0.5f + 0.5f * std::sin(time * 5);
        SDL_SetTextureAlphaMod(sprites.ring.get(), static_cast<Uint8>(150 + 105 * pulse));
        for (const auto& cell : winLine->cells) {
            const Object& piece = *pieces[cell.row][cell.col];
            Texture::drawCentered(renderer, sprites.ring.get(), piece.x(), piece.y(), scale);
        }
        SDL_SetTextureAlphaMod(sprites.ring.get(), 255);
    }
}
