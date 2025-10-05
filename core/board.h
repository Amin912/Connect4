#pragma once

#include <array>
#include <optional>
#include <string_view>

namespace connect4 {

enum class Player : unsigned char { None, One, Two };

constexpr Player opponent(Player p)
{
    return p == Player::One ? Player::Two : Player::One;
}

struct Cell {
    int row;
    int col;
};

// Four connected pieces of one player, used to show where a game was won.
struct Line {
    Player owner;
    std::array<Cell, 4> cells;
};

// The 6x7 grid and the rules for dropping pieces into it. It knows nothing
// about rendering, so the AI and the tests can use it without SDL.
//
// Row 0 is the top row, matching how the board is drawn; pieces fill each
// column from row 5 upwards. Columns are numbered 0 to 6.
class Board {
public:
    static constexpr int Rows = 6;
    static constexpr int Cols = 7;

    // Builds a position from a move list written as column digits 1-7,
    // e.g. "4453". Players alternate, starting with Player::One.
    // Throws std::invalid_argument for a bad digit or a full column.
    static Board fromMoves(std::string_view moves);

    Player at(int row, int col) const { return cells_[row][col]; }
    bool canPlay(int col) const { return col >= 0 && col < Cols && heights_[col] < Rows; }
    bool isFull() const { return moveCount_ == Rows * Cols; }
    int moveCount() const { return moveCount_; }

    // Drops a piece into a column that has room and returns the row it lands in.
    int play(int col, Player player);

    // Takes back the top piece of a column. The search uses play/undo pairs
    // to explore moves without copying the board.
    void undo(int col);

    // True if the piece at (row, col) is part of four in a row. Checking only
    // the lines through the last move is much cheaper than scanning the board.
    bool connectsFour(int row, int col) const;

    // Scans the whole board for four in a row.
    std::optional<Line> findFour() const;

private:
    std::array<std::array<Player, Cols>, Rows> cells_{};
    std::array<int, Cols> heights_{};
    int moveCount_ = 0;
};

}  // namespace connect4
