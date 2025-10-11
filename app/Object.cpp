#include "Object.h"

#include "Game.h"

Object::Object(int x, int y, int w, int h, SDL_Texture* texture)
    : texture(texture), dest{x, y, w, h}
{
}

void Object::fall(int floorY)
{
    const int restY = floorY - dest.h;
    if (dest.y >= restY) {
        return;
    }
    // Speed grows by one pixel per frame. The position is clamped in the
    // same step, so a piece is never drawn below its row for a frame.
    ++yspeed;
    dest.y += yspeed;
    if (dest.y >= restY) {
        dest.y = restY;
        yspeed = 0;
    }
}

void Object::render() const
{
    SDL_RenderCopy(Game::renderer, texture, nullptr, &dest);
}
