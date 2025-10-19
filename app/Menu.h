#pragma once

#include "Button.h"
#include "Paint.h"
#include "Settings.h"
#include "Text.h"

#include <SDL.h>

#include <vector>

// The start screen: pick a difficulty and who moves first, then play.
class Menu
{
public:
    enum class Action { None, Play, Quit };

    Menu(SDL_Renderer* renderer, const Sprites& sprites, Text& text);

    Action event(const SDL_Event& e);
    void update(float dt);
    // Returns true if the mouse is over a button.
    bool render() const;

    const Settings& settings() const { return chosen; }

private:
    SDL_Renderer* renderer;
    const Sprites& sprites;
    Text& text;
    Settings chosen;
    float time = 0;

    std::vector<Button> difficultyButtons;  // Easy, Medium, Hard
    std::vector<Button> firstMoveButtons;   // You, AI
    Button playButton;
};
