#pragma once

#include "core/board.h"

#include <SDL.h>

// Colours and layout shared by the screens. Positions and sizes are in window
// units; on high-DPI displays the renderer scales them to pixels.
namespace theme {

constexpr int WindowWidth = 840;
constexpr int WindowHeight = 640;

constexpr SDL_Color BackgroundTop{15, 20, 36, 255};
constexpr SDL_Color BackgroundBottom{27, 33, 56, 255};
constexpr SDL_Color BoardFront{52, 66, 112, 255};
constexpr SDL_Color BoardBack{12, 16, 30, 255};
constexpr SDL_Color HumanColor{255, 107, 107, 255};  // coral
constexpr SDL_Color AiColor{56, 214, 200, 255};      // aqua
constexpr SDL_Color TextColor{238, 241, 248, 255};
constexpr SDL_Color MutedTextColor{139, 147, 173, 255};

constexpr float Cell = 64;
constexpr float BoardPadding = 18;
constexpr float BoardCorner = 22;
constexpr float HoleRadius = 25;
// Slightly larger than a hole, so a piece's edge stays hidden behind the board.
constexpr float PieceRadius = 27;

constexpr float BoardWidth = connect4::Board::Cols * Cell + 2 * BoardPadding;
constexpr float BoardHeight = connect4::Board::Rows * Cell + 2 * BoardPadding;
constexpr float BoardX = (WindowWidth - BoardWidth) / 2;
constexpr float BoardY = 140;

// Centre line of the strip above the board where a piece is held before it drops.
constexpr float DropZoneY = BoardY - 40;

constexpr float columnCenterX(int col) { return BoardX + BoardPadding + (col + 0.5f) * Cell; }
constexpr float rowCenterY(int row) { return BoardY + BoardPadding + (row + 0.5f) * Cell; }

}  // namespace theme
