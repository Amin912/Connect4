#include "Texture.h"

SDL_Texture* Texture::loadTexture(const char* file){
    SDL_Texture* ret;
    SDL_Surface* tmp=IMG_Load(file);
    ret=SDL_CreateTextureFromSurface(Game::renderer,tmp);
    SDL_FreeSurface(tmp);
    return ret;
}
