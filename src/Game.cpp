#include "Game.h"
#include "Texture.h"
#include "Object.h"
#include "Grid.h"
//Grid* gr;
Object* menu;
Grid* grid;
int height;
int width;
SDL_Renderer* Game::renderer=nullptr;
Game::Game(){}

void Game::init(char* title, int x, int y, int w, int h, bool fullscreen)
{
    height=h;
    width=w;
    int flag=fullscreen?SDL_WINDOW_FULLSCREEN:0;

    if(SDL_Init(SDL_INIT_EVERYTHING)==0){
        cout<<"SDL initialted"<<endl;

        window=SDL_CreateWindow(title, x, y, w, h, flag);
        if (window){cout<<"window created"<<endl;}

        renderer= SDL_CreateRenderer(window, -1, 0);
        if (renderer){cout<<"renderer created"<<endl;SDL_SetRenderDrawColor(renderer, 255, 215, 100, 25);}
        isActivated=true;

    }
    //grid=new Grid(grid);
    //g=new Object(240,200,600,500, "assets/grid.png");
    char* path1="assets/R.png";
    char* path2="assets/CC.png";
    char* path3="assets/menu.jpg";
    grid=new Grid(path1,path2);
    menu= new Object(0,0,width,height,path3);
    //texture =Texture::loadTexture(, renderer);


}

void Game::event()
{
    SDL_Event event;
    SDL_PollEvent(&event);
    if(!menuOn)
        grid->event(event);

    switch(event.type){
    case SDL_QUIT:
        isActivated=false;
        break;
    if(menuOn){

    case SDL_MOUSEBUTTONDOWN:
        int x,y;
        SDL_GetMouseState(&x,&y);
        if (x<width-30)
            menuOn=false;
            break;
    }
//    case SDL_MOUSEBUTTONDOWN:
//        int x,y;
//        SDL_GetMouseState(&x,&y);
//        cout<<x<<endl<<y<<endl;
//        break;
    default:
        break;

    }
}
void Game::render()
{
    SDL_RenderClear(renderer);
    if(menuOn)
    {
       menu->render();
    }
    else
    {
        //menu->render();
        grid->render();

    }
    SDL_RenderPresent(renderer);
}

void Game::update()
{
    if(menuOn)
    {
        menu->update();
    }
    else{
        //menu->update();
        grid->update();
    }
}

void Game::over()
{
    SDL_DestroyWindow(window);
    SDL_DestroyRenderer(renderer);
    SDL_Quit();
    cout<<"Game ended"<<endl;
}


Game::~Game()
{
    //dtor
}
