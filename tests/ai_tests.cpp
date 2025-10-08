#include "core/ai.h"

#include <doctest/doctest.h>

#include <random>

using connect4::Board;
using connect4::Player;
using connect4::opponent;
namespace ai = connect4::ai;

namespace {

// A random position reached without anyone winning, and whose turn it is.
struct Position {
    Board board;
    Player toMove = Player::One;
};

Position randomPosition(std::mt19937& rng, int maxMoves)
{
    std::uniform_int_distribution<int> anyColumn(0, Board::Cols - 1);
    std::uniform_int_distribution<int> length(0, maxMoves);
    for (;;) {
        Position pos;
        const int moves = length(rng);
        bool won = false;
        for (int i = 0; i < moves && !won; ++i) {
            int col = anyColumn(rng);
            while (!pos.board.canPlay(col)) {
                col = anyColumn(rng);
            }
            const int row = pos.board.play(col, pos.toMove);
            won = pos.board.connectsFour(row, col);
            pos.toMove = opponent(pos.toMove);
        }
        if (!won) {
            return pos;
        }
    }
}

}  // namespace

TEST_CASE("takes a win in the last column rather than blocking")
{
    // Two has three stacked in column 7. One threatens the bottom row at
    // column 4, but Two moves first and can simply win.
    const Board board = Board::fromMoves("1717273");
    for (int depth = 1; depth <= 6; ++depth) {
        CAPTURE(depth);
        const auto result = ai::bestMove(board, Player::Two, {depth});
        CHECK(result.column == 6);
        CHECK(result.score == ai::WinScore + depth);
    }
}

TEST_CASE("blocks a win the opponent threatens next move")
{
    // One has three stacked in column 4; Two has nothing to win with.
    const Board board = Board::fromMoves("41424");
    for (int depth = 2; depth <= 6; ++depth) {
        CAPTURE(depth);
        CHECK(ai::bestMove(board, Player::Two, {depth}).column == 3);
    }
}

TEST_CASE("returns no move on a full board")
{
    Board board;
    Player player = Player::One;
    for (int col = 0; col < Board::Cols; ++col) {
        for (int i = 0; i < Board::Rows; ++i) {
            board.play(col, player);
            player = opponent(player);
        }
    }
    CHECK(ai::bestMove(board, Player::One).column == -1);
}

TEST_CASE("evaluation is zero-sum")
{
    CHECK(ai::evaluate(Board{}, Player::One) == 0);

    std::mt19937 rng(7);
    for (int i = 0; i < 200; ++i) {
        const Position pos = randomPosition(rng, 30);
        CHECK(ai::evaluate(pos.board, Player::One) == -ai::evaluate(pos.board, Player::Two));
    }
}

TEST_CASE("minimax and alpha-beta choose the same move")
{
    // Alpha-beta is only an optimisation: it must never change the answer.
    std::mt19937 rng(12345);
    for (int i = 0; i < 25; ++i) {
        const Position pos = randomPosition(rng, 20);
        for (int depth = 1; depth <= 5; ++depth) {
            for (bool centerFirst : {true, false}) {
                CAPTURE(i);
                CAPTURE(depth);
                CAPTURE(centerFirst);
                const auto slow = ai::bestMove(pos.board, pos.toMove,
                                               {depth, ai::Algorithm::Minimax, centerFirst});
                const auto fast = ai::bestMove(pos.board, pos.toMove,
                                               {depth, ai::Algorithm::AlphaBeta, centerFirst});
                CHECK(fast.column == slow.column);
                CHECK(fast.score == slow.score);
                CHECK(fast.nodes <= slow.nodes);
            }
        }
    }
}

TEST_CASE("alpha-beta visits far fewer positions than minimax")
{
    const auto slow = ai::bestMove(Board{}, Player::One, {6, ai::Algorithm::Minimax});
    const auto fast = ai::bestMove(Board{}, Player::One, {6, ai::Algorithm::AlphaBeta});
    CHECK(fast.nodes * 5 < slow.nodes);
}
