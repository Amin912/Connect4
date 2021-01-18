#ifndef GAME_H
#define GAME_H
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <cstring>

using namespace std;
class Game
{
    public:
        Game();
        void init(char*, int, int, int, int, bool); //creates the game window
        void event(); //handles player inputs
        void render(); //updates the
        void update();
        void over();
        bool activated(){return isActivated;}
        virtual ~Game();

        static SDL_Renderer* renderer;
    protected:

    private:
        SDL_Window* window;
        bool isActivated;
        bool menuOn=1;

};

#endif // GAME_H
