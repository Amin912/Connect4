#ifndef GRID_H
#define GRID_H
#include "Object.h"
#define minfinity -10000
#define pinfinity 10000
#define HEIGHT 700

class Grid
{
    public:
        Grid()
        {
            gameOff=false;
        }
        Grid(const char* path1, const char* path2)
        {
            gameOff=false;
            loadChecks(path1, path2);
            gridShape=new Object(gridX,gridY,600,500, "assets/grid.png");
            w1=new Object(0,0,900,700,"assets/grid.png");
//            w2=new Object(0,0,900,700,"assets/grid.png");
        }
        void loadChecks(const char* , const char* path2);
        void render();
        void update();
        bool putCheck(int);
        int TestThread(void*);
        int normalize(int);
        int denormalize(int);
        int quantisize(int);
        void event(SDL_Event);
        bool win();
        int evaluate();
        void scoreVal(int& , int , int);
        int geturn(){return turn;}
        void next(){turn=turn==1?2:1;}
        bool validCol(int col){return state[0][col]==0;}
        Grid** children(int);
        int minimax(int ,bool);
        int alphabeta(int, int, int ,bool);
        int aiInput(bool, int);
        //Grid** children(int);
        virtual ~Grid();

    protected:

    private:
        Object* gridShape; //defines the size and texture of the grid
        Object* w1; //player1 wins screen
        Object* w2; //player2 wins screen
        bool gameOff; //true if one player wins
        int turn=1;  //next plater to play
        int state[6][7]={}; //represents the grid state
        int gridX=150;//Modify to change the grid position
        int gridY=150;//
        int checkSize=80;
        Object* checkers1[6][7];
        Object* checkers2[6][7];
};

#endif // GRID_H
