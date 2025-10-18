#pragma once

#include <SDL.h>

#include <memory>

struct TextureDeleter {
    void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
};

struct SurfaceDeleter {
    void operator()(SDL_Surface* surface) const { SDL_FreeSurface(surface); }
};

// Owning pointers that free the SDL object when they go out of scope.
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;
using SurfacePtr = std::unique_ptr<SDL_Surface, SurfaceDeleter>;

class Texture
{
public:
    // Uploads a surface to the GPU, with alpha blending and smooth scaling.
    // Throws std::runtime_error on failure.
    static TexturePtr fromSurface(SDL_Renderer* renderer, SDL_Surface* surface);

    // Draw a texture that was made at `scale` pixels per window unit, so it
    // maps one to one onto screen pixels. Positions are in window units.
    static void draw(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float scale);
    static void drawCentered(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y,
                             float scale);
};
