#pragma once

#include "Text.h"
#include "Texture.h"

#include <SDL.h>

#include <string>

// A rounded button. Its shape is painted once in white and tinted when drawn,
// so the hover and selected states need no extra images.
class Button
{
public:
    enum class Style {
        Primary,    // filled with the accent colour
        Secondary,  // subtle, for less important actions
        Option,     // one choice in a group, e.g. a difficulty level
    };

    Button(SDL_Renderer* renderer, float scale, std::string label, SDL_FRect area, Style style);

    bool contains(float x, float y) const;
    bool clicked(const SDL_Event& e) const;  // a left click inside the button

    // Returns true if the mouse is over the button.
    bool render(Text& text, bool selected = false) const;

private:
    SDL_Renderer* renderer;
    float scale;
    std::string label;
    SDL_FRect area;
    Style style;
    TexturePtr shape;
};
