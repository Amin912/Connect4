#include "core/board.h"

#include <doctest/doctest.h>

#include <stdexcept>
#include <string_view>

using connect4::Board;
using connect4::Player;

TEST_CASE("pieces stack from the bottom row up")
{
    Board board;
    CHECK(board.play(3, Player::One) == 5);
    CHECK(board.play(3, Player::Two) == 4);
    CHECK(board.at(5, 3) == Player::One);
    CHECK(board.at(4, 3) == Player::Two);
    CHECK(board.at(3, 3) == Player::None);
    CHECK(board.moveCount() == 2);
}

TEST_CASE("a full column takes no more pieces")
{
    Board board = Board::fromMoves("111111");
    CHECK_FALSE(board.canPlay(0));
    CHECK(board.canPlay(1));
    CHECK_FALSE(board.canPlay(-1));
    CHECK_FALSE(board.canPlay(Board::Cols));
}

TEST_CASE("undo takes back the top piece")
{
    Board board = Board::fromMoves("44");
    board.undo(3);
    CHECK(board.at(4, 3) == Player::None);
    CHECK(board.at(5, 3) == Player::One);
    CHECK(board.moveCount() == 1);
    CHECK(board.play(3, Player::Two) == 4);
}

TEST_CASE("fromMoves rejects bad input")
{
    CHECK_THROWS_AS(Board::fromMoves("8"), std::invalid_argument);
    CHECK_THROWS_AS(Board::fromMoves("0"), std::invalid_argument);
    CHECK_THROWS_AS(Board::fromMoves("1111111"), std::invalid_argument);
}

TEST_CASE("the board is full after 42 moves")
{
    Board board;
    Player player = Player::One;
    for (int col = 0; col < Board::Cols; ++col) {
        for (int i = 0; i < Board::Rows; ++i) {
            CHECK_FALSE(board.isFull());
            board.play(col, player);
            player = connect4::opponent(player);
        }
    }
    CHECK(board.isFull());
}

TEST_CASE("four in a row is found in every direction")
{
    struct Example {
        const char* name;
        const char* moves;  // Player One's last move completes the four
    };
    const Example examples[] = {
        {"horizontal", "1122334"},
        {"vertical", "1212121"},
        {"diagonal", "12233434474"},
        {"anti-diagonal", "76655454414"},
    };
    for (const auto& example : examples) {
        CAPTURE(example.name);
        const Board board = Board::fromMoves(example.moves);
        const auto line = board.findFour();
        REQUIRE(line.has_value());
        CHECK(line->owner == Player::One);

        const int lastCol = std::string_view(example.moves).back() - '1';
        int lastRow = 0;
        while (board.at(lastRow, lastCol) == Player::None) {
            ++lastRow;
        }
        CHECK(board.connectsFour(lastRow, lastCol));
    }
}

TEST_CASE("three in a row is not a win")
{
    const Board board = Board::fromMoves("112233");
    CHECK_FALSE(board.findFour().has_value());
    for (int col = 0; col < 3; ++col) {
        CHECK_FALSE(board.connectsFour(5, col));
    }
}

TEST_CASE("a line does not wrap from one row to the next")
{
    // One owns columns 5-7 of the second row from the bottom and column 1 of
    // the bottom row. Stored row after row, these are four consecutive cells.
    const Board board = Board::fromMoves("1556677");
    CHECK_FALSE(board.findFour().has_value());
}
