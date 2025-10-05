#include "core/board.h"

#include <cassert>
#include <stdexcept>
#include <string>

namespace connect4 {

namespace {

// The four directions a line can run in. The opposite directions are covered
// by walking each line both ways.
constexpr int Directions[4][2] = {
    {0, 1},   // horizontal
    {1, 0},   // vertical
    {1, 1},   // diagonal, down-right
    {1, -1},  // diagonal, down-left
};

bool inside(int row, int col)
{
    return row >= 0 && row < Board::Rows && col >= 0 && col < Board::Cols;
}

}  // namespace

Board Board::fromMoves(std::string_view moves)
{
    Board board;
    Player player = Player::One;
    for (char digit : moves) {
        const int col = digit - '1';
        if (col < 0 || col >= Cols) {
            throw std::invalid_argument(std::string("bad column '") + digit + "'");
        }
        if (!board.canPlay(col)) {
            throw std::invalid_argument(std::string("column ") + digit + " is full");
        }
        board.play(col, player);
        player = opponent(player);
    }
    return board;
}

int Board::play(int col, Player player)
{
    assert(canPlay(col));
    assert(player != Player::None);
    const int row = Rows - 1 - heights_[col];
    cells_[row][col] = player;
    ++heights_[col];
    ++moveCount_;
    return row;
}

void Board::undo(int col)
{
    assert(col >= 0 && col < Cols && heights_[col] > 0);
    --heights_[col];
    --moveCount_;
    cells_[Rows - 1 - heights_[col]][col] = Player::None;
}

bool Board::connectsFour(int row, int col) const
{
    const Player owner = at(row, col);
    if (owner == Player::None) {
        return false;
    }
    for (const auto& dir : Directions) {
        int count = 1;
        for (int sign : {1, -1}) {
            int r = row + sign * dir[0];
            int c = col + sign * dir[1];
            while (inside(r, c) && at(r, c) == owner) {
                ++count;
                r += sign * dir[0];
                c += sign * dir[1];
            }
        }
        if (count >= 4) {
            return true;
        }
    }
    return false;
}

std::optional<Line> Board::findFour() const
{
    for (int row = 0; row < Rows; ++row) {
        for (int col = 0; col < Cols; ++col) {
            const Player owner = at(row, col);
            if (owner == Player::None) {
                continue;
            }
            for (const auto& dir : Directions) {
                Line line{owner, {}};
                int k = 0;
                for (; k < 4; ++k) {
                    const int r = row + k * dir[0];
                    const int c = col + k * dir[1];
                    if (!inside(r, c) || at(r, c) != owner) {
                        break;
                    }
                    line.cells[k] = {r, c};
                }
                if (k == 4) {
                    return line;
                }
            }
        }
    }
    return std::nullopt;
}

}  // namespace connect4
