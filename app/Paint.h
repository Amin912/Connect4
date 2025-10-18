#pragma once

#include "Texture.h"

#include <SDL.h>

// Images drawn in code instead of loaded from files. Each shape is described
// by its distance to an edge, which gives smooth anti-aliased edges at any
// size. Sizes are in window units; `scale` is pixels per unit, so the result
// stays sharp on high-DPI screens.
namespace paint {

SurfacePtr piece(float radius, SDL_Color color, float scale);

// The front of the board with its holes cut out, and the dark panel behind it.
// Pieces are drawn between the two, so they appear to slide down inside.
SurfacePtr boardFront(float scale);
SurfacePtr boardBack(float scale);

// Soft shadow under the board, `blur` units wider than it on every side.
SurfacePtr boardShadow(float blur, float scale);

// White outline used to highlight winning pieces.
SurfacePtr ring(float radius, float thickness, float scale);

// White rounded rectangle, tinted at draw time for buttons.
SurfacePtr roundedRect(float width, float height, float corner, float scale);

// Soft white blob that fades to transparent, tinted for background lighting.
SurfacePtr glow(float radius, float scale);

}  // namespace paint

// The textures every screen uses, created once at startup.
struct Sprites {
    Sprites(SDL_Renderer* renderer, float scale);

    float scale;
    TexturePtr humanPiece;
    TexturePtr aiPiece;
    TexturePtr boardFront;
    TexturePtr boardBack;
    TexturePtr boardShadow;
    TexturePtr ring;
    TexturePtr glow;

    static constexpr float ShadowBlur = 26;
    static constexpr float RingRadius = 31;
    static constexpr float GlowRadius = 260;
};
