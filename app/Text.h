#pragma once

#include "Texture.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include <map>
#include <string>
#include <tuple>
#include <utility>

enum class Weight { Medium, SemiBold, Bold };
enum class Align { Left, Center, Right };

// Draws text in the bundled Poppins font. Fonts are opened at the screen's
// pixel density so text is as sharp as the rest of the UI. Each string is
// rendered once in white, cached, and tinted when drawn.
class Text
{
public:
    // Throws std::runtime_error if SDL_ttf or a font file fails to load.
    Text(SDL_Renderer* renderer, std::string fontDir, float scale);
    ~Text();
    Text(const Text&) = delete;
    Text& operator=(const Text&) = delete;

    // Draws text vertically centred on y. x is the left edge, centre or
    // right edge depending on align. Returns the width in window units.
    float draw(const std::string& text, Weight weight, int size, SDL_Color color, float x, float y,
               Align align = Align::Left);

    float width(const std::string& text, Weight weight, int size);

private:
    TTF_Font* font(Weight weight, int size);
    SDL_Texture* rendered(const std::string& text, Weight weight, int size);

    SDL_Renderer* renderer;
    std::string fontDir;
    float scale;
    std::map<std::pair<Weight, int>, TTF_Font*> fonts;
    std::map<std::tuple<std::string, Weight, int>, TexturePtr> cache;
};
