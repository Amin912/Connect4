#include "Button.h"

#include "Paint.h"
#include "Theme.h"

namespace {

constexpr SDL_Color DarkText{22, 26, 44, 255};

}  // namespace

Button::Button(SDL_Renderer* renderer, float scale, std::string label, SDL_FRect area, Style style)
    : renderer(renderer),
      scale(scale),
      label(std::move(label)),
      area(area),
      style(style),
      shape(Texture::fromSurface(renderer, paint::roundedRect(area.w, area.h, area.h / 2, scale).get()))
{
}

bool Button::contains(float x, float y) const
{
    return x >= area.x && x < area.x + area.w && y >= area.y && y < area.y + area.h;
}

bool Button::clicked(const SDL_Event& e) const
{
    return e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT &&
           contains(static_cast<float>(e.button.x), static_cast<float>(e.button.y));
}

bool Button::render(Text& text, bool selected) const
{
    int mouseX = 0;
    int mouseY = 0;
    SDL_GetMouseState(&mouseX, &mouseY);
    const bool hovered = contains(static_cast<float>(mouseX), static_cast<float>(mouseY));

    SDL_Color fill{255, 255, 255, 255};
    SDL_Color labelColor = theme::TextColor;
    Weight weight = Weight::SemiBold;
    int size = 16;

    switch (style) {
    case Style::Primary:
        fill = theme::HumanColor;
        if (hovered) {
            fill = {255, 135, 135, 255};
        }
        labelColor = DarkText;
        weight = Weight::Bold;
        size = 18;
        break;
    case Style::Secondary:
        fill.a = hovered ? 36 : 20;
        break;
    case Style::Option:
        if (selected) {
            fill = theme::TextColor;
            labelColor = DarkText;
        } else {
            fill.a = hovered ? 30 : 14;
            labelColor = hovered ? theme::TextColor : theme::MutedTextColor;
        }
        size = 15;
        break;
    }

    SDL_SetTextureColorMod(shape.get(), fill.r, fill.g, fill.b);
    SDL_SetTextureAlphaMod(shape.get(), fill.a);
    Texture::draw(renderer, shape.get(), area.x, area.y, scale);
    text.draw(label, weight, size, labelColor, area.x + area.w / 2, area.y + area.h / 2 + 1, Align::Center);
    return hovered;
}
