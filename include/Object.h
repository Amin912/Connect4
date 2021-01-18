#ifndef OBJECT_H
#define OBJECT_H
#include "Game.h"
#include "Texture.h"

class Object
{
    public:
        Object(int, int, int, int, const char*);
        ~Object();
        void update();
        void render();
        void fall(const int);
        void setX(int x){xpos=x;}
        void setY(int y){ypos=y;}
        int getX(){return xpos;}
        int getY(){return ypos;}

    protected:

    private:
        int xpos;
        int ypos;
        int yspeed=0;
        SDL_Texture* texture;
        SDL_Rect src, dest;

};

#endif // OBJECT_H
