#pragma once

#include <SDL.h>

#include <memory>
#include <string>

struct TextureDeleter {
    void operator()(SDL_Texture* texture) const { SDL_DestroyTexture(texture); }
};

// A texture that is destroyed automatically when it goes out of scope.
using TexturePtr = std::unique_ptr<SDL_Texture, TextureDeleter>;

class Texture
{
public:
    // Loads an image file. Throws std::runtime_error if the file is missing
    // or cannot be decoded.
    static TexturePtr loadTexture(const std::string& file);
};
