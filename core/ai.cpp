#include "core/ai.h"

#include <algorithm>
#include <array>

namespace connect4::ai {

namespace {

constexpr int Infinity = 1 << 30;

// Score for a window holding 0-4 pieces of one player and none of the other.
// A full window is a win, which the search detects and scores on its own.
constexpr int WindowScore[5] = {0, 1, 15, 40, 0};

constexpr std::array<int, Board::Cols> CenterFirst = {3, 2, 4, 1, 5, 0, 6};
constexpr std::array<int, Board::Cols> LeftToRight = {0, 1, 2, 3, 4, 5, 6};

// Value of a win found with `depth` moves of search left. A win found with
// more depth left is closer to the current position, so it scores higher:
// the AI takes the quickest win and puts off a loss as long as it can.
int winValue(bool forMaximizer, int depth)
{
    return forMaximizer ? WinScore + depth : -(WinScore + depth);
}

class Search {
public:
    Search(Player me, const Options& options)
        : me_(me), order_(options.centerFirst ? CenterFirst : LeftToRight)
    {
    }

    const std::array<int, Board::Cols>& order() const { return order_; }
    std::uint64_t nodes() const { return nodes_; }

    int minimax(Board& board, int depth, Player toMove)
    {
        ++nodes_;
        if (board.isFull()) {
            return 0;
        }
        if (depth == 0) {
            return evaluate(board, me_);
        }

        const bool maximizing = toMove == me_;
        int best = maximizing ? -Infinity : Infinity;
        for (int col : order_) {
            if (!board.canPlay(col)) {
                continue;
            }
            const int row = board.play(col, toMove);
            const int value = board.connectsFour(row, col)
                                  ? winValue(maximizing, depth)
                                  : minimax(board, depth - 1, opponent(toMove));
            board.undo(col);
            best = maximizing ? std::max(best, value) : std::min(best, value);
        }
        return best;
    }

    // Same result as minimax, but stops exploring a move as soon as it is
    // clear the other player would never allow it. alpha is the best score
    // the maximizer is already guaranteed elsewhere, beta the same for the
    // minimizer; once alpha >= beta the rest of this node cannot matter.
    int alphaBeta(Board& board, int depth, int alpha, int beta, Player toMove)
    {
        ++nodes_;
        if (board.isFull()) {
            return 0;
        }
        if (depth == 0) {
            return evaluate(board, me_);
        }

        const bool maximizing = toMove == me_;
        int best = maximizing ? -Infinity : Infinity;
        for (int col : order_) {
            if (!board.canPlay(col)) {
                continue;
            }
            const int row = board.play(col, toMove);
            const int value = board.connectsFour(row, col)
                                  ? winValue(maximizing, depth)
                                  : alphaBeta(board, depth - 1, alpha, beta, opponent(toMove));
            board.undo(col);
            if (maximizing) {
                best = std::max(best, value);
                alpha = std::max(alpha, best);
            } else {
                best = std::min(best, value);
                beta = std::min(beta, best);
            }
            if (alpha >= beta) {
                break;
            }
        }
        return best;
    }

private:
    Player me_;
    std::array<int, Board::Cols> order_;
    std::uint64_t nodes_ = 0;
};

}  // namespace

int evaluate(const Board& board, Player me)
{
    constexpr int Directions[4][2] = {{0, 1}, {1, 0}, {1, 1}, {1, -1}};
    const Player them = opponent(me);
    int score = 0;
    for (const auto& dir : Directions) {
        for (int row = 0; row < Board::Rows; ++row) {
            for (int col = 0; col < Board::Cols; ++col) {
                const int lastRow = row + 3 * dir[0];
                const int lastCol = col + 3 * dir[1];
                if (lastRow >= Board::Rows || lastCol < 0 || lastCol >= Board::Cols) {
                    continue;
                }
                int mine = 0;
                int theirs = 0;
                for (int k = 0; k < 4; ++k) {
                    const Player p = board.at(row + k * dir[0], col + k * dir[1]);
                    mine += p == me;
                    theirs += p == them;
                }
                if (theirs == 0) {
                    score += WindowScore[mine];
                } else if (mine == 0) {
                    score -= WindowScore[theirs];
                }
            }
        }
    }
    return score;
}

Result bestMove(Board board, Player me, const Options& options)
{
    const int depth = std::max(options.depth, 1);
    Search search(me, options);
    Result result;

    // The root is a maximizing node whose moves are compared directly.
    // Passing the best score so far as alpha lets alpha-beta cut the other
    // moves short; it still returns the exact score for any move that beats
    // it, so both algorithms pick the same column.
    int alpha = -Infinity;
    for (int col : search.order()) {
        if (!board.canPlay(col)) {
            continue;
        }
        const int row = board.play(col, me);
        int value = 0;
        if (board.connectsFour(row, col)) {
            value = winValue(true, depth);
        } else if (options.algorithm == Algorithm::Minimax) {
            value = search.minimax(board, depth - 1, opponent(me));
        } else {
            value = search.alphaBeta(board, depth - 1, alpha, Infinity, opponent(me));
        }
        board.undo(col);

        if (result.column == -1 || value > result.score) {
            result.column = col;
            result.score = value;
            alpha = std::max(alpha, value);
        }
    }
    result.nodes = search.nodes();
    return result;
}

}  // namespace connect4::ai
