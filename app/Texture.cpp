#include "Texture.h"

#include "Game.h"

#include <SDL_image.h>

#include <stdexcept>

TexturePtr Texture::loadTexture(const std::string& file)
{
    SDL_Texture* texture = IMG_LoadTexture(Game::renderer, file.c_str());
    if (!texture) {
        throw std::runtime_error("Could not load " + file + ": " + IMG_GetError());
    }
    return TexturePtr(texture);
}
