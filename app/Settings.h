#pragma once

enum class Difficulty { Easy, Medium, Hard };

// What the player picked on the menu.
struct Settings {
    Difficulty difficulty = Difficulty::Medium;
    bool humanFirst = true;
};

// How many moves ahead the AI looks at each level.
constexpr int searchDepth(Difficulty difficulty)
{
    switch (difficulty) {
    case Difficulty::Easy:
        return 2;
    case Difficulty::Medium:
        return 4;
    case Difficulty::Hard:
        return 8;
    }
    return 4;
}

constexpr const char* difficultyName(Difficulty difficulty)
{
    switch (difficulty) {
    case Difficulty::Easy:
        return "Easy";
    case Difficulty::Medium:
        return "Medium";
    case Difficulty::Hard:
        return "Hard";
    }
    return "";
}
