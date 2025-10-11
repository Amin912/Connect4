#pragma once

#include <SDL.h>

// A textured rectangle on screen: the board image or one piece. Pieces fall
// under gravity until they reach their row. The texture is shared between
// objects and owned elsewhere.
class Object
{
public:
    Object(int x, int y, int w, int h, SDL_Texture* texture);

    // Moves the object one frame's worth of gravity towards floorY, the
    // y coordinate its bottom edge should rest on.
    void fall(int floorY);
    void render() const;

    const SDL_Rect& rect() const { return dest; }

private:
    SDL_Texture* texture;
    SDL_Rect dest;
    int yspeed = 0;
};
