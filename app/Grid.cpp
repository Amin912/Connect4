#include "Grid.h"

#include "Theme.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <string>

using connect4::Board;
using connect4::Player;

namespace {

// Shortest pause before the AI replies, so the player's piece lands first.
constexpr float AiDelaySeconds = 0.55f;

constexpr float Centre = theme::WindowWidth / 2.f;
constexpr float HeaderY = 44;
constexpr float FooterY = 604;

}  // namespace

Grid::Grid(SDL_Renderer* renderer, const Sprites& sprites, Text& text)
    : renderer(renderer),
      sprites(sprites),
      text(text),
      menuButton(renderer, sprites.scale, "Menu", SDL_FRect{24, HeaderY - 18, 88, 36},
                 Button::Style::Secondary),
      playAgainButton(renderer, sprites.scale, "Play again", SDL_FRect{Centre - 100, 574, 200, 50},
                      Button::Style::Primary)
{
}

void Grid::start(const Settings& chosen)
{
    settings = chosen;
    aiOptions.depth = searchDepth(settings.difficulty);
    humanWins = 0;
    aiWins = 0;
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
        startAiTurn();
    }
}

void Grid::startAiTurn()
{
    // Search on a background thread so the window keeps drawing; at higher
    // difficulty a move can take a noticeable fraction of a second. The
    // search gets its own copy of the board.
    state = State::AiTurn;
    aiDelay = AiDelaySeconds;
    aiMove = std::async(std::launch::async, [position = board, options = aiOptions] {
        return connect4::ai::bestMove(position, Ai, options);
    });
}

int Grid::columnAt(float x, float y) const
{
    const float offset = x - theme::BoardX - theme::BoardPadding;
    if (offset < 0 || y < theme::DropZoneY - theme::PieceRadius ||
        y > theme::BoardY + theme::BoardHeight) {
        return -1;
    }
    const int col = static_cast<int>(offset / theme::Cell);
    return col < Board::Cols ? col : -1;
}

Grid::Action Grid::event(const SDL_Event& e)
{
    if (menuButton.clicked(e)) {
        return Action::Menu;
    }
    if (state == State::Over) {
        const bool enter = e.type == SDL_KEYDOWN &&
                           (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE);
        if (playAgainButton.clicked(e) || enter) {
            restart();
        }
        return Action::None;
    }

    switch (e.type) {
    case SDL_MOUSEMOTION:
        selectedColumn = columnAt(static_cast<float>(e.motion.x), static_cast<float>(e.motion.y));
        break;
    case SDL_MOUSEBUTTONDOWN:
        if (e.button.button == SDL_BUTTON_LEFT) {
            selectedColumn = columnAt(static_cast<float>(e.button.x), static_cast<float>(e.button.y));
            dropSelected();
        }
        break;
    case SDL_KEYDOWN: {
        const SDL_Keycode key = e.key.keysym.sym;
        if (key >= SDLK_1 && key <= SDLK_7) {
            selectedColumn = key - SDLK_1;
            dropSelected();
        } else if (key >= SDLK_KP_1 && key <= SDLK_KP_7) {
            selectedColumn = key - SDLK_KP_1;
            dropSelected();
        } else if (key == SDLK_LEFT) {
            selectedColumn = selectedColumn < 0 ? 3 : std::max(selectedColumn - 1, 0);
        } else if (key == SDLK_RIGHT) {
            selectedColumn = selectedColumn < 0 ? 3 : std::min(selectedColumn + 1, Board::Cols - 1);
        } else if (key == SDLK_DOWN || key == SDLK_RETURN || key == SDLK_SPACE) {
            dropSelected();
        }
        break;
    }
    default:
        break;
    }
    return Action::None;
}

void Grid::dropSelected()
{
    if (state == State::HumanTurn && board.canPlay(selectedColumn)) {
        play(selectedColumn, Human);
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
        ++(human ? humanWins : aiWins);
    } else if (board.isFull()) {
        state = State::Over;
    } else if (human) {
        startAiTurn();
    } else {
        state = State::HumanTurn;
    }
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
        const bool searchDone = aiMove.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
        if (aiDelay <= 0 && searchDone) {
            play(aiMove.get().column, Ai);
        }
    }
}

bool Grid::drawHeader() const
{
    const bool overMenu = menuButton.render(text);

    // Status in the middle, with a dot in the colour of whoever it concerns.
    std::string status;
    std::string dots;
    SDL_Texture* dot = sprites.humanDot.get();
    SDL_Color color = theme::TextColor;
    if (state == State::HumanTurn) {
        status = "Your turn";
    } else if (state == State::AiTurn) {
        status = "AI is thinking";
        dots = std::string(static_cast<std::size_t>(time * 3) % 4, '.');
        dot = sprites.aiDot.get();
    } else if (!winLine) {
        status = "It's a draw";
        dot = nullptr;
        color = theme::MutedTextColor;
    } else if (winLine->owner == Human) {
        status = "You win!";
        color = theme::HumanColor;
    } else {
        status = "The AI wins";
        dot = sprites.aiDot.get();
        color = theme::AiColor;
    }
    const int size = 22;
    const float width = text.width(status, Weight::SemiBold, size) + (dot ? 22 : 0);
    float x = Centre - width / 2;
    if (dot) {
        Texture::drawCentered(renderer, dot, x + 7, HeaderY, sprites.scale);
        x += 22;
    }
    x += text.draw(status, Weight::SemiBold, size, color, x, HeaderY);
    text.draw(dots, Weight::SemiBold, size, color, x, HeaderY);

    // Score and difficulty on the right.
    const float right = theme::WindowWidth - 28;
    const std::string score = "You " + std::to_string(humanWins) + "  \xE2\x80\x93  " +
                              std::to_string(aiWins) + " AI";
    text.draw(score, Weight::SemiBold, 16, theme::TextColor, right, HeaderY - 9, Align::Right);
    text.draw(difficultyName(settings.difficulty), Weight::Medium, 13, theme::MutedTextColor, right,
              HeaderY + 13, Align::Right);
    return overMenu;
}

void Grid::drawPreview() const
{
    if (state != State::HumanTurn || !board.canPlay(selectedColumn)) {
        return;
    }
    const float x = theme::columnCenterX(selectedColumn);
    SDL_Texture* piece = sprites.humanPiece.get();
    SDL_SetTextureAlphaMod(piece, 170);
    Texture::drawCentered(renderer, piece, x, theme::DropZoneY + 3 * std::sin(time * 4), sprites.scale);
    SDL_SetTextureAlphaMod(piece, 255);
}

bool Grid::drawFooter() const
{
    if (state == State::Over) {
        return playAgainButton.render(text);
    }
    text.draw("Click a column or press 1\xE2\x80\x93" "7  \xC2\xB7  Esc for menu", Weight::Medium, 13,
              theme::MutedTextColor, Centre, FooterY, Align::Center);
    return false;
}

bool Grid::render() const
{
    const float scale = sprites.scale;
    Texture::draw(renderer, sprites.boardShadow.get(), theme::BoardX - Sprites::ShadowBlur,
                  theme::BoardY - Sprites::ShadowBlur + 14, scale);
    Texture::draw(renderer, sprites.boardBack.get(), theme::BoardX, theme::BoardY, scale);

    // Light up the empty holes of the column the player is aiming at. Drawn
    // behind the front of the board, so it only shows through the holes.
    if (state == State::HumanTurn && board.canPlay(selectedColumn)) {
        SDL_Texture* strip = sprites.columnHighlight.get();
        SDL_SetTextureColorMod(strip, theme::HumanColor.r, theme::HumanColor.g, theme::HumanColor.b);
        SDL_SetTextureAlphaMod(strip, 40);
        Texture::drawCentered(renderer, strip, theme::columnCenterX(selectedColumn),
                              theme::BoardY + theme::BoardHeight / 2, scale);
        SDL_SetTextureColorMod(strip, 255, 255, 255);
        SDL_SetTextureAlphaMod(strip, 255);
    }

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

    drawPreview();
    bool clickable = drawHeader();
    clickable |= drawFooter();
    clickable |= state == State::HumanTurn && board.canPlay(selectedColumn);
    return clickable;
}
