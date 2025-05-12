#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 10
#define H 20

#define F(x,y) for(int x=0;x<y;x++)
#define EQ(a,b) ((a)==(b))

int _0[H][W]={0},_1=W/2,_2=0,_3=0,_4=0;

int _5[2][2]={{1,1},{1,1}};

#define D() do{\
clear();\
F(i,W+2)printw("#");printw("\n");\
F(y,H){printw("#");F(x,W){\
if(_0[y][x])printw("#");\
else if(y>=_2&&y<_2+2&&x>=_1&&x<_1+2&&_5[y-_2][x-_1])printw("O");\
else printw(" ");}printw("#\n");}\
F(i,W+2)printw("#");printw("\nS:%d\n",_4);refresh();\
}while(0)

#define C(nx,ny) ({int c=0;F(i,2)F(j,2)if(_5[i][j]){\
int fx=nx+j,fy=ny+i;\
if(fx<0||fx>=W||fy>=H||_0[fy][fx]){c=1;}}c;})

#define M() do{F(i,2)F(j,2)if(_5[i][j]){\
int fx=_1+j,fy=_2+i;\
if(fx<W&&fy<H)_0[fy][fx]=1;}}while(0)

#define CL() do{\
for(int i=H-1;i>=0;i--){\
int f=1;F(j,W)if(!_0[i][j])f=0;\
if(f){_4+=100;for(int k=i;k>0;k--)F(j,W)_0[k][j]=_0[k-1][j];i++;}}\
}while(0)

#define IN() do{\
timeout(0);int k=getch();\
(k=='a'&&!C(_1-1,_2))?_1--:0;\
(k=='d'&&!C(_1+1,_2))?_1++:0;\
(k=='s'&&!C(_1,_2+1))?_2++:0;\
(k=='x')?(_3=1):0;\
}while(0)

#define L() do{\
if(!C(_1,_2+1))_2++;\
else{M();CL();_1=W/2;_2=0;if(C(_1,_2))_3=1;}\
}while(0)

int main(){initscr();noecho();curs_set(0);srand(time(0));
while(!_3){D();IN();L();usleep(200000);}
D();printw("X! S:%d\n",_4);printw("Key...");timeout(-1);getch();endwin();return 0;}
