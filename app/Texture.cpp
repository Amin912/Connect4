#include "Texture.h"

#include <stdexcept>
#include <string>

TexturePtr Texture::fromSurface(SDL_Renderer* renderer, SDL_Surface* surface)
{
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        throw std::runtime_error(std::string("Could not create a texture: ") + SDL_GetError());
    }
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    return TexturePtr(texture);
}

void Texture::draw(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y, float scale)
{
    int width = 0;
    int height = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    const SDL_FRect dest{x, y, width / scale, height / scale};
    SDL_RenderCopyF(renderer, texture, nullptr, &dest);
}

void Texture::drawCentered(SDL_Renderer* renderer, SDL_Texture* texture, float x, float y,
                           float scale)
{
    int width = 0;
    int height = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    draw(renderer, texture, x - width / scale / 2, y - height / scale / 2, scale);
}
