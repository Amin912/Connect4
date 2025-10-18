#pragma once

#include <SDL.h>

// A piece on screen. It falls under gravity until it reaches its row, then
// bounces once. The texture is shared between pieces and owned elsewhere.
class Object
{
public:
    // (x, y) is the centre, in window units.
    Object(float x, float y, SDL_Texture* texture);

    // Advances the fall by dt seconds. restY is where the centre comes to rest.
    void fall(float restY, float dt);
    void render(SDL_Renderer* renderer, float scale, Uint8 alpha = 255) const;

    bool resting() const { return atRest; }
    float x() const { return xpos; }
    float y() const { return ypos; }

private:
    SDL_Texture* texture;
    float xpos;
    float ypos;
    float yspeed = 0;  // units per second, downwards
    bool atRest = false;
};
