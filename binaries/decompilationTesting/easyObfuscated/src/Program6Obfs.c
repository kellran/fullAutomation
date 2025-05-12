#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

#define W 40
#define H 20
#define PW 7
#define R 3
#define C 10

#define L(x) ((x)+1)
#define F(x,y) for(int x=0;x<y;x++)
#define S(x,y) ((x)<(y))
#define EQ(x,y) ((x)==(y))

int _0=W/2, _1=H-5, _2=1, _3=-1, _4=W/2-PW/2, _5[R][C], _6=0, _7=0;

#define B() do{F(i,R)F(j,C)_5[i][j]=1;}while(0)

#define D() do{\
clear();\
F(i,W+2) printw("#"); printw("\n");\
F(y,H){printw("#");F(x,W){int f=0;\
F(i,R){F(j,C){int bx=j*(W/C),by=i+1;\
if(x>=bx&&x<bx+(W/C)&&EQ(y,by)&&_5[i][j]){printw("*");f=1;break;}}if(f)break;}\
if(!f&&EQ(x,_0)&&EQ(y,_1))printw("O");\
else if(!f&&EQ(y,H-1)&&x>=_4&&x<_4+PW)printw("=");\
else if(!f)printw(" ");}printw("#\n");}\
F(i,W+2) printw("#");\
printw("\nS:%d\n",_6);\
refresh();}while(0)

#define I() do{timeout(0);int k=getch();\
(k=='a'&&_4>0)?_4--:0;\
(k=='d'&&_4+PW<W)?_4++:0;\
(k=='x')?(_7=1):0;\
}while(0)

#define LOG() do{\
_0+=_2; _1+=_3;\
if(_0<=0||_0>=W-1) _2*=-1;\
if(_1<=0) _3*=-1;\
if(_1==H-2&&_0>=_4&&_0<_4+PW) _3*=-1;\
F(i,R)F(j,C){int bx=j*(W/C),by=i+1;\
if(_5[i][j]&&_1==by&&_0>=bx&&_0<bx+(W/C)){\
_5[i][j]=0; _3*=-1; _6+=10;}}\
if(_1>=H-1) _7=1;\
}while(0)

int main(){initscr();noecho();curs_set(0);B();
while(!_7){D();I();LOG();usleep(50000);}
D();printw("\nX! S:%d\n",_6);printw("Key...");timeout(-1);getch();endwin();return 0;}
