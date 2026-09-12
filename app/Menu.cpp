#include "Menu.h"

#include "Theme.h"

#include <algorithm>
#include <cmath>

namespace {

constexpr float Centre = theme::WindowWidth / 2.f;
constexpr float OptionWidth = 116;
constexpr float OptionHeight = 40;
constexpr float OptionGap = 8;

// A row of option buttons centred in the window.
std::vector<Button> optionRow(SDL_Renderer* renderer, float scale, std::initializer_list<const char*> labels,
                              float y)
{
    const float count = static_cast<float>(labels.size());
    float x = Centre - (count * OptionWidth + (count - 1) * OptionGap) / 2;
    std::vector<Button> buttons;
    for (const char* label : labels) {
        buttons.emplace_back(renderer, scale, label, SDL_FRect{x, y, OptionWidth, OptionHeight},
                             Button::Style::Option);
        x += OptionWidth + OptionGap;
    }
    return buttons;
}

}  // namespace

Menu::Menu(SDL_Renderer* renderer, const Sprites& sprites, Text& text)
    : renderer(renderer),
      sprites(sprites),
      text(text),
      difficultyButtons(optionRow(renderer, sprites.scale, {"Easy", "Medium", "Hard"}, 358)),
      firstMoveButtons(optionRow(renderer, sprites.scale, {"You", "AI"}, 444)),
      playButton(renderer, sprites.scale, "Play", SDL_FRect{Centre - 110, 520, 220, 54},
                 Button::Style::Primary)
{
}

Menu::Action Menu::event(const SDL_Event& e)
{
    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_RETURN:
        case SDLK_KP_ENTER:
        case SDLK_SPACE:
            return Action::Play;
        case SDLK_ESCAPE:
            return Action::Quit;
        case SDLK_LEFT:
        case SDLK_RIGHT: {
            const int step = e.key.keysym.sym == SDLK_LEFT ? -1 : 1;
            const int level = std::clamp(static_cast<int>(chosen.difficulty) + step, 0, 2);
            chosen.difficulty = static_cast<Difficulty>(level);
            return Action::None;
        }
        case SDLK_UP:
        case SDLK_DOWN:
            chosen.humanFirst = !chosen.humanFirst;
            return Action::None;
        default:
            return Action::None;
        }
    }

    for (std::size_t i = 0; i < difficultyButtons.size(); ++i) {
        if (difficultyButtons[i].clicked(e)) {
            chosen.difficulty = static_cast<Difficulty>(i);
        }
    }
    if (firstMoveButtons[0].clicked(e)) {
        chosen.humanFirst = true;
    }
    if (firstMoveButtons[1].clicked(e)) {
        chosen.humanFirst = false;
    }
    return playButton.clicked(e) ? Action::Play : Action::None;
}

void Menu::update(float dt)
{
    time += dt;
}

bool Menu::render() const
{
    // Four pieces in a row, gently bobbing like a wave.
    for (int i = 0; i < 4; ++i) {
        const float x = Centre + (i - 1.5f) * theme::Cell;
        const float y = 146 + 6 * std::sin(time * 2.2f - i * 0.7f);
        Texture::drawCentered(renderer, sprites.humanPiece.get(), x, y, sprites.scale);
    }

    text.draw("Four in a Row", Weight::Bold, 54, theme::TextColor, Centre, 236, Align::Center);
    text.draw("Line up four of your pieces before the AI does.", Weight::Medium, 17,
              theme::MutedTextColor, Centre, 282, Align::Center);

    bool hovered = false;
    text.draw("DIFFICULTY", Weight::SemiBold, 13, theme::MutedTextColor, Centre, 338, Align::Center);
    for (std::size_t i = 0; i < difficultyButtons.size(); ++i) {
        hovered |= difficultyButtons[i].render(text, chosen.difficulty == static_cast<Difficulty>(i));
    }

    text.draw("FIRST MOVE", Weight::SemiBold, 13, theme::MutedTextColor, Centre, 424, Align::Center);
    hovered |= firstMoveButtons[0].render(text, chosen.humanFirst);
    hovered |= firstMoveButtons[1].render(text, !chosen.humanFirst);

    hovered |= playButton.render(text);

#ifdef __EMSCRIPTEN__
    const char* hint = "Arrow keys to choose  \xC2\xB7  Enter to play";  // a web page can't be quit
#else
    const char* hint = "Arrow keys to choose  \xC2\xB7  Enter to play  \xC2\xB7  Esc to quit";
#endif
    text.draw(hint, Weight::Medium, 13, theme::MutedTextColor, Centre, 610, Align::Center);
    return hovered;
}
