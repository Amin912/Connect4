// Compares how many positions each search variant visits to choose a move.
// Prints a Markdown table, ready to paste into the README.

#include "core/ai.h"

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <string_view>

using connect4::Board;
using connect4::Player;
namespace ai = connect4::ai;

namespace {

struct Variant {
    const char* name;
    ai::Algorithm algorithm;
    bool centerFirst;
};

constexpr Variant Variants[] = {
    {"Minimax", ai::Algorithm::Minimax, false},
    {"Alpha-beta", ai::Algorithm::AlphaBeta, false},
    {"Alpha-beta, centre first", ai::Algorithm::AlphaBeta, true},
};

// Opening positions as move lists (columns 1-7). The totals below add up the
// work for all of them.
constexpr std::string_view Positions[] = {"", "4453", "445362", "4453627115"};

}  // namespace

int main()
{
    std::printf("| Depth |");
    for (const auto& variant : Variants) {
        std::printf(" %s |", variant.name);
    }
    std::printf("\n|---:|");
    for (std::size_t i = 0; i < std::size(Variants); ++i) {
        std::printf("---:|");
    }
    std::printf("\n");

    for (int depth = 1; depth <= 8; ++depth) {
        std::printf("| %d |", depth);
        for (const auto& variant : Variants) {
            std::uint64_t nodes = 0;
            const auto start = std::chrono::steady_clock::now();
            for (std::string_view moves : Positions) {
                const Board board = Board::fromMoves(moves);
                const Player toMove = moves.size() % 2 == 0 ? Player::One : Player::Two;
                nodes += ai::bestMove(board, toMove, {depth, variant.algorithm, variant.centerFirst}).nodes;
            }
            const auto ms = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start);
            std::printf(" %llu (%.1f ms) |", static_cast<unsigned long long>(nodes), ms.count());
        }
        std::printf("\n");
        std::fflush(stdout);
    }
    return 0;
}
