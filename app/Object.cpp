#include "Object.h"

#include "Texture.h"

namespace {

constexpr float Gravity = 3800;     // units per second squared
constexpr float Bounce = 0.22f;     // share of the speed kept after hitting the bottom
constexpr float MinBounceSpeed = 250;

}  // namespace

Object::Object(float x, float y, SDL_Texture* texture) : texture(texture), xpos(x), ypos(y) {}

void Object::fall(float restY, float dt)
{
    if (atRest) {
        return;
    }
    yspeed += Gravity * dt;
    ypos += yspeed * dt;
    if (ypos >= restY) {
        // Clamp in the same step, so a piece is never drawn below its row.
        ypos = restY;
        if (yspeed > MinBounceSpeed) {
            yspeed = -yspeed * Bounce;
        } else {
            yspeed = 0;
            atRest = true;
        }
    }
}

void Object::render(SDL_Renderer* renderer, float scale, Uint8 alpha) const
{
    SDL_SetTextureAlphaMod(texture, alpha);
    Texture::drawCentered(renderer, texture, xpos, ypos, scale);
    SDL_SetTextureAlphaMod(texture, 255);
}
