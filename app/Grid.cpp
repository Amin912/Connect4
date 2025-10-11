#include "Grid.h"

#include "Game.h"

using connect4::Board;
using connect4::Player;

namespace {

// Left edge of each column in the board image, relative to gridX. The image
// is drawn at 600x500, so the first column is slightly wider than the rest.
constexpr int ColumnEdges[Board::Cols + 1] = {0, 89, 172, 255, 338, 421, 504, 587};

// Frames to wait before the AI replies, so the player's piece lands first.
constexpr int AiDelayFrames = 35;

constexpr const char* DefaultTitle = "Connect 4";

}  // namespace

Grid::Grid(const std::string& assetDir)
    : humanTexture(Texture::loadTexture(assetDir + "R.png")),
      aiTexture(Texture::loadTexture(assetDir + "CC.png")),
      gridTexture(Texture::loadTexture(assetDir + "grid.png")),
      gridShape(std::make_unique<Object>(gridX, gridY, 600, 500, gridTexture.get()))
{
}

int Grid::columnAt(int x) const
{
    const int offset = x - gridX;
    for (int col = 0; col < Board::Cols; ++col) {
        if (offset >= ColumnEdges[col] && offset < ColumnEdges[col + 1]) {
            return col;
        }
    }
    return -1;
}

int Grid::columnX(int col) const
{
    return gridX + (col == 0 ? 5 : ColumnEdges[col]);
}

int Grid::floorY(int row) const
{
    return gridY + 490 - (Board::Rows - 1 - row) * checkSize;
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
        const int col = columnAt(e.button.x);
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
        columnX(col), human ? 60 : 40, checkSize, checkSize,
        human ? humanTexture.get() : aiTexture.get());

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
        aiDelay = AiDelayFrames;
    } else {
        state = State::HumanTurn;
    }
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
    state = State::HumanTurn;
    setTitle(DefaultTitle);
}

void Grid::setTitle(const char* title) const
{
    SDL_SetWindowTitle(SDL_RenderGetWindow(Game::renderer), title);
}

void Grid::update()
{
    for (int row = 0; row < Board::Rows; ++row) {
        for (int col = 0; col < Board::Cols; ++col) {
            if (pieces[row][col]) {
                pieces[row][col]->fall(floorY(row));
            }
        }
    }

    if (state == State::AiTurn && --aiDelay <= 0) {
        const auto move = connect4::ai::bestMove(board, Ai, aiOptions);
        play(move.column, Ai);
    }
}

void Grid::render() const
{
    // Pieces first, then the board on top: its holes are transparent, so the
    // pieces show through them.
    for (const auto& row : pieces) {
        for (const auto& piece : row) {
            if (piece) {
                piece->render();
            }
        }
    }
    gridShape->render();

    if (winLine) {
        Uint8 r, g, b, a;
        SDL_GetRenderDrawColor(Game::renderer, &r, &g, &b, &a);
        SDL_SetRenderDrawColor(Game::renderer, 40, 220, 90, 255);
        for (const auto& cell : winLine->cells) {
            SDL_Rect outline = pieces[cell.row][cell.col]->rect();
            for (int i = 0; i < 4; ++i) {
                SDL_RenderDrawRect(Game::renderer, &outline);
                outline = {outline.x - 1, outline.y - 1, outline.w + 2, outline.h + 2};
            }
        }
        SDL_SetRenderDrawColor(Game::renderer, r, g, b, a);
    }
}
