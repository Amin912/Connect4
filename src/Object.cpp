#include "Object.h"


Object::Object(int x, int y, int w, int h, const char* file)
{
    xpos=x;
    ypos=y;
    //dest.x=xpos;dest.y=ypos;
    dest.w=w;
    dest.h=h;
    texture= Texture::loadTexture(file);
}
void Object::fall(const int floorY)
{
    int acc;
    if (ypos<floorY-dest.h)acc=1;
    else {acc=0;yspeed=0;ypos=floorY-dest.h;}
    yspeed+=acc;
}

void Object::update()
{
    ypos+=yspeed;
    src.x=0;
    src.y=0;
    src.w=110;
    src.h=60;
    //src=NULL;
    dest.x=xpos;
    dest.y=ypos;


}

void Object::render()
{

    SDL_RenderCopy(Game::renderer, texture, NULL, &dest);
}

Object::~Object()
{
    //delete[] renderer;
    //delete[] texture;
}
