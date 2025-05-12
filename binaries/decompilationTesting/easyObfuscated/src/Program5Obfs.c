#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define W 40
#define H 20
#define G 6
#define P 3

#define X(x) ((x)+1)
#define Z(x,y) for(int x=0;x<y;x++)
#define S(x,y) ((x)<(y))
#define C(x,y) ((x)==(y))

int _0, _1 = H/2, _2 = 5, _3 = 0, _4 = 1, _5 = 0, _6 = W, _7, _8 = 0;

#define R() (rand() % (H - G - 2) + 1)
#define INIT() do{ srand(time(0)); _7=R(); }while(0)

#define DRAWW() do{\
clear();\
Z(i,H){Z(j,W){\
C(j,0)||C(j,W-1)?printw("#"):\
(C(j,_2)&&C(i,_1)?printw("O"):\
(j>=_6&&j<_6+P&&(i<_7||i>_7+G)?printw("|"):printw(" ")));}printw("\n");}\
Z(k,W) printw("#");\
printw("\nS:%d\n",_5);\
refresh();\
}while(0)

#define IN() do{ timeout(0); int k=getch();\
k==' '?(_3=-3):(k=='x'?(_8=1):0);\
}while(0)

#define LOGIC() do{\
_3+=_4; _1+=_3;\
((_1<=0)||(_1>=H-1))&&(_8=1);\
_6--;\
if(_6+P<0){_6=W;_7=R();_5++;}\
if(_2>=_6&&_2<_6+P&&(_1<_7||_1>_7+G)) _8=1;\
}while(0)

int main(){
    initscr();noecho();curs_set(FALSE);nodelay(stdscr,TRUE);
    INIT();
    while(!_8){ DRAWW(); IN(); LOGIC(); usleep(100000); }
    DRAWW();
    printw("\nX! S:%d\n",_5);
    printw("Key...");
    timeout(-1); getch(); endwin();
    return 0;
}
