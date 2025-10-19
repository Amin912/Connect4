#include "Text.h"

#include <cmath>
#include <stdexcept>

namespace {

const char* fileName(Weight weight)
{
    switch (weight) {
    case Weight::Medium:
        return "Poppins-Medium.ttf";
    case Weight::SemiBold:
        return "Poppins-SemiBold.ttf";
    case Weight::Bold:
        return "Poppins-Bold.ttf";
    }
    return "Poppins-Medium.ttf";
}

}  // namespace

Text::Text(SDL_Renderer* renderer, std::string fontDir, float scale)
    : renderer(renderer), fontDir(std::move(fontDir)), scale(scale)
{
    if (TTF_Init() != 0) {
        throw std::runtime_error(std::string("Could not start SDL_ttf: ") + TTF_GetError());
    }
}

Text::~Text()
{
    cache.clear();
    for (auto& entry : fonts) {
        TTF_CloseFont(entry.second);
    }
    TTF_Quit();
}

TTF_Font* Text::font(Weight weight, int size)
{
    const auto key = std::make_pair(weight, size);
    if (auto it = fonts.find(key); it != fonts.end()) {
        return it->second;
    }
    const std::string path = fontDir + fileName(weight);
    TTF_Font* opened = TTF_OpenFont(path.c_str(), static_cast<int>(std::lround(size * scale)));
    if (!opened) {
        throw std::runtime_error("Could not load " + path + ": " + TTF_GetError());
    }
    fonts.emplace(key, opened);
    return opened;
}

SDL_Texture* Text::rendered(const std::string& text, Weight weight, int size)
{
    const auto key = std::make_tuple(text, weight, size);
    if (auto it = cache.find(key); it != cache.end()) {
        return it->second.get();
    }
    const SurfacePtr surface(TTF_RenderUTF8_Blended(font(weight, size), text.c_str(), {255, 255, 255, 255}));
    if (!surface) {
        throw std::runtime_error(std::string("Could not render text: ") + TTF_GetError());
    }
    TexturePtr texture = Texture::fromSurface(renderer, surface.get());
    SDL_Texture* result = texture.get();
    cache.emplace(key, std::move(texture));
    return result;
}

float Text::width(const std::string& text, Weight weight, int size)
{
    int w = 0;
    SDL_QueryTexture(rendered(text, weight, size), nullptr, nullptr, &w, nullptr);
    return w / scale;
}

float Text::draw(const std::string& text, Weight weight, int size, SDL_Color color, float x, float y,
                 Align align)
{
    if (text.empty()) {
        return 0;
    }
    SDL_Texture* texture = rendered(text, weight, size);
    int w = 0;
    int h = 0;
    SDL_QueryTexture(texture, nullptr, nullptr, &w, &h);
    const float width = w / scale;
    const float height = h / scale;
    const float left = align == Align::Left ? x : align == Align::Center ? x - width / 2 : x - width;

    SDL_SetTextureColorMod(texture, color.r, color.g, color.b);
    SDL_SetTextureAlphaMod(texture, color.a);
    Texture::draw(renderer, texture, left, y - height / 2, scale);
    return width;
}
