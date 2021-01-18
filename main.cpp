//#define SDL_MAIN_HANDLED

#include <iostream>
#include "Game.h"
#define HEIGHT 700
using namespace std;


Game* game;

int main (int argc, char** argv) {
    game=new Game();
    cout<<"1.Start game"<<endl;
    int menu=1;
    //cin>>menu;
    if (menu==1){
    game->init("CONNECT4", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 900, HEIGHT, false); //true for fullscreen

    //normalizing frame count
    int fps=60;
    int frameDuration=1000/fps;
    Uint32 frameStart;
    int deltaTime;
    //cout<<game->activated();
    while (game->activated())
    {
        frameStart=SDL_GetTicks();
        game->event();
        game->update();
        game->render();

        deltaTime=SDL_GetTicks()-frameStart;
        if(deltaTime<frameDuration)SDL_Delay(frameDuration-deltaTime);

    }
    game->over();
    }
    return 0;
}
