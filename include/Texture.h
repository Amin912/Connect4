#ifndef TEXTURE_H
#define TEXTURE_H
#include <iostream>
using namespace std;
#include <SDL.h>
#include <SDL_image.h>
#include "Game.h"

class Texture
{
    public:
        static SDL_Texture* loadTexture(const char* file);
};

#endif // TEXTURE_H
