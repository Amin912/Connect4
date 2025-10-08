#pragma once

#include "core/board.h"

#include <cstdint>

namespace connect4::ai {

enum class Algorithm { Minimax, AlphaBeta };

struct Options {
    // How many moves ahead to look, counting the AI's own move.
    int depth = 6;
    Algorithm algorithm = Algorithm::AlphaBeta;
    // Try the central columns first. Central moves are usually stronger, and
    // finding a strong move early lets alpha-beta skip more of the tree.
    bool centerFirst = true;
};

struct Result {
    int column = -1;          // -1 only when the board is full
    int score = 0;
    std::uint64_t nodes = 0;  // positions visited, for comparing algorithms
};

// Any score at or above this means a forced win was found.
constexpr int WinScore = 1'000'000;

// Heuristic value of a position for `me`: every window of four cells that
// holds pieces of only one player scores 1, 15 or 40 for 1, 2 or 3 pieces,
// positive for `me` and negative for the opponent.
int evaluate(const Board& board, Player me);

// Chooses the column for `me` to play.
Result bestMove(Board board, Player me, const Options& options = {});

}  // namespace connect4::ai
