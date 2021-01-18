#include "Grid.h"
int nodeCount=0;
int skipDuration=35;
int skipTime=0;
void Grid::loadChecks(const char* path1, const char* path2)
{
    for (int i=5;i>=0;i--){
            for(int j=0;j<7;j++)
            {
                    checkers1[i][j]=new Object(denormalize(j+1),60,checkSize,checkSize,path1);checkers2[i][j]=new Object(denormalize(j+1),40,checkSize,checkSize,"assets/CC.png");
            }
            }
}
int Grid::normalize(int position)
{
    if (position>gridX && position<gridX+89)return 1;
    if (position>gridX+89 && position<gridX+172)return 2;
    if (position>gridX+172 && position<gridX+255)return 3;
    if (position>gridX+255 && position<gridX+338)return 4;
    if (position>gridX+338 && position<gridX+421)return 5;
    if (position>gridX+421 && position<gridX+504)return 6;
    if (position>gridX+504 && position<gridX+587)return 7;
    return 0;
}
int Grid::denormalize(int p)
{
    if (p==1)return gridX+5;
    if (p==2)return gridX+89;
    if (p==3)return gridX+172;
    if (p==4)return gridX+255;
    if (p==5)return gridX+338;
    if (p==6)return gridX+421;
    if (p==7)return gridX+504;
    return 0;
}
int Grid::quantisize(int position)
{
    if (position>gridX && position<gridX+89)return gridX;
    if (position>gridX+89 && position<gridX+172)return gridX+89;
    if (position>gridX+172 && position<gridX+255)return gridX+172;
    if (position>gridX+255 && position<gridX+338)return gridX+255;
    if (position>gridX+338 && position<gridX+421)return gridX+338;
    if (position>gridX+421 && position<gridX+504)return gridX+421;
    if (position>gridX+504 && position<gridX+587)return gridX+504;
    return 0;
}

bool Grid::putCheck(int pos)
{
    int position;
    if (pos<7 && pos>0) position=pos;
    else position=normalize(pos);
//    if (turn=1)position=normalize(pos);
//    else position=pos;
    if(position){
    //if (state[0][position-1]!=0) cout<<"overload "<<turn<<endl;
    for(int i=5;i>=0;i--){
        if (state[i][position-1]==0){
            state[i][position-1]=turn;
            return true;
        }
    }
    }
    return false;
}


void Grid::event(SDL_Event e)
{
    if (skipTime>skipDuration+1) skipTime=0;
    if (!gameOff){
        if (turn==1){
    switch(e.type)
    {


        case SDL_MOUSEBUTTONDOWN:
            int x,y,xx;
            SDL_GetMouseState(&x,&y);
            cout<<x<<endl;
            //putCheck(x);
            xx=quantisize(x);
            if (xx && putCheck(x)){
            if (normalize(x))next();
            skipTime=0;
            }
            break;
            default:break;
    }
        }

            else {
                cout<<skipTime<<endl;
                if (skipTime>skipDuration){putCheck(aiInput(true,6)); next();}
         }

        }
            else {state[6][7]={};gameOff=false;}
    skipTime++;

}

void Grid::render()
{
    win();
    if(!gameOff)
    {

//    int k=0;
//    int n=0;

    for (int i=5;i>=0;i--){
        for(int j=0;j<7;j++){
            if(state[i][j]==1)
            {
                checkers1[i][j]->render();
            }
            if(state[i][j]==2)
            {
                checkers2[i][j]->render();
            }
        }
    }
    gridShape->render();
    }
    else{w1->render();}


}

void Grid::update()
{
       // win();

       // gridShape->update();
//        int k=0;
//        int n=0;
        if (!gameOff){
        for (int i=5;i>=0;i--){
            for(int j=0;j<7;j++){
                if(state[i][j]==1)
                {
                    checkers1[i][j]->update();
                    //checkers1[i][j]->setY(170+i*86);

                    checkers1[i][j]->fall(gridY+490-(5-i)*checkSize);

                }
                if(state[i][j]==2)
                {

                    checkers2[i][j]->update();
                    //checkers2[n++]->setY(170+i*86);

                    checkers2[i][j]->fall(gridY+490-(5-i)*checkSize);
                }
            }
        }
        //cout<<checkers2[0]->getY()<<endl;
        gridShape->update();
        }
        else {w1->update();}


    //cout<<gameOff;

}

bool Grid::win(){
    bool win=false;
    for (int i=0;i<3;i++){
        for(int j=0;j<7;j++){
            if(state[i][j]!=0 && state[i][j]==state[i+1][j] && state[i][j]==state[i+2][j] && state[i][j]==state[i+3][j])win=true;
        }
    }

    for (int i=0;i<6;i++){
        for(int j=0;j<4;j++){
            if(state[i][j]!=0 && state[i][j]==state[i][j+1] && state[i][j]==state[i][j+2] && state[i][j]==state[i][j+3])win=true;
        }
    }
    for (int i=0;i<3;i++){
        for(int j=0;j<4;j++){
            if(state[i][j]!=0 && state[i][j]==state[i+1][j+1] && state[i][j]==state[i+2][j+2] && state[i][j]==state[i+3][j+3])win=true;
        }
    }
    for (int i=0;i<3;i++){
        for(int j=3;j<7;j++){
            if(state[i][j]!=0 && state[i][j]==state[i+1][j-1] && state[i][j]==state[i+2][j-2] && state[i][j]==state[i+3][j-3])win=true;
        }
    }

    gameOff=win;
    return win;
}

int Grid::evaluate()
{
    int score=0;
    int cmax=0,cmin=0;
    for(int i=0;i<3;i++){
            for(int j=0;j<7;j++){
                    for(int k=0;k<4;k++){
                            {
                                if(state[i+k][j]==1) cmin++;
                                if(state[i+k][j]==2) cmax++;
                            }
                    }
                    scoreVal(score, cmax,cmin);
                    cmax=0;cmin=0;
            }
    }

    for(int i=0;i<6;i++){
        for(int j=0;j<4;j++){
                    for(int k=0;k<4;k++){
                            {
                                if(state[i][j+k]==1) cmin++;
                                if(state[i][j+k]==2) cmax++;
                            }
                    }
                    scoreVal(score, cmax,cmin);cmax=0;cmin=0;
            }
    }

    for (int i=0;i<3;i++){
        for(int j=0;j<4;j++){
                    for(int k=0;k<4;k++){
                            {
                                if(state[i+k][j+k]==1) cmin++;
                                if(state[i+k][j+k]==2) cmax++;
                            }
                    }
                    scoreVal(score, cmax,cmin);
                    cmax=0;
                    cmin=0;
            }
    }

    for (int i=0;i<3;i++){
        for(int j=3;j<7;j++){
                    for(int k=0;k<4;k++){
                            {
                                if(state[i+k][j-k]==1) cmin++;
                                if(state[i+k][j-k]==2) cmax++;
                            }
                    }
                    scoreVal(score, cmax, cmin);cmax=0;cmin=0;
            }
    }
    return score;
}

void Grid::scoreVal(int &score, int cmax , int cmin){
    if(cmin==0){
        switch(cmax){
            case 1:
                score+=1;
                break;
            case 2:
                score+=15;
                break;
            case 3:
                score+=40
                ;
                break;
        }
    }
    if (cmax==0){
        switch(cmin){
            case 1:
                score-=1;
                break;
            case 2:
                score-=15;
                break;
            case 3:
                score-=40;
                break;
        }
    }

}
///kznfojkzomeknc

Grid** Grid::children(int t){
    //nodeCount+=7;
    Grid **result;
    result= new Grid*[7];
    for (int i=0;i<7;i++){
        result[i]=new Grid();
        for (int j=0;j<6;j++){
                for (int k=0;k<7;k++){result[i]->state[j][k]=state[j][k];}}
        result[i]->turn=t;
        result[i]->putCheck(i+1);
    }
    return result;
}

int Grid::minimax(int depth , bool maxplay){
    int value;
    if(win() &&turn==1) return minfinity;
    if(win() && turn ==2) return pinfinity;
    if (depth==0){
        return evaluate();
    }

    if (maxplay){
        value=minfinity;
        Grid** childList= new Grid*[7];
        childList=children(2);
        for(int i=0; i<7;i++) value=max(value,childList[i]->minimax(depth-1 ,false));
        return value;
    }
    else {
        value=pinfinity;
        Grid** childList= new Grid*[7];
        childList=children(1);
        for(int i=0; i<7;i++) value=min(value, childList[i]->minimax(depth-1,true));
        return value;
    }
}
int Grid::alphabeta(int depth , int alpha, int beta, bool maxplay){
    int value;
    if(win() &&turn==1) return minfinity;
    if(win() && turn ==2) return pinfinity;
    if (depth==0){
        return evaluate();
    }

    if (maxplay){
        value=minfinity;
        Grid** childList= new Grid*[7];
        childList=children(2);
        for(int i=0; i<7;i++){
                value=max(value,childList[i]->minimax(depth-1 ,false));
                alpha=max(alpha, value);
                if (alpha>beta)break;
        }
        return value;
    }
    else {
        value=pinfinity;
        Grid** childList= new Grid*[7];
        childList=children(1);
        for(int i=0; i<7;i++){
            value=min(value, childList[i]->minimax(depth-1,true));
            beta=min(value, beta);
            if(alpha>beta)break;
        }
        return value;
    }
}


int Grid::aiInput(bool alphabetaAlgo, int depth)
{
    int in;
    int mini=minfinity;
    Grid** successors=new Grid*[7];
    successors=children(2);
    if (alphabetaAlgo)
    {
        for (int i=0;i<7;i++){
            if(mini<successors[i]->alphabeta(depth-1, minfinity, pinfinity, false )&& validCol(i))
            {
                mini=successors[i]->alphabeta(depth-1,minfinity, pinfinity, false);
                in=i+1;
            }
        }
    }
    else
    {
        for (int i=0;i<7;i++){
        if(mini<successors[i]->minimax(depth-1, false) && validCol(i))
            {
                mini=successors[i]->minimax(depth-1, false);
                in=i+1;
            }
        }
    }
    return in;

}

Grid::~Grid()
{
    //dtor
}
