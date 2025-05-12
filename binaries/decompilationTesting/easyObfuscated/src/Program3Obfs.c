#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define MB 5
#define ME 10

#define F(x,y) for(int x=0;x<y;x++)
#define EQ(a,b) ((a)==(b))

int _0=W/2,_1[MB][2],_2[ME][2],_3=1,_4=0,_5=0;

#define I() do{\
F(i,MB)_1[i][0]=-1;\
F(i,ME){_2[i][0]=2+i/5;_2[i][1]=5+(i%5)*6;}\
}while(0)

#define D() do{\
clear();\
F(y,H){F(x,W){char p=0;\
if(x==0||x==W-1){printw("#");continue;}\
if(y==H-1&&x==_0){printw("A");continue;}\
F(i,ME){if(_2[i][0]==y&&_2[i][1]==x){printw("M");p=1;break;}}\
if(!p){F(b,MB){if(_1[b][0]==y&&_1[b][1]==x){printw("|");p=1;break;}}}\
if(!p)printw(" ");}printw("\n");}\
printw("S:%d\n",_4);refresh();\
}while(0)

#define IN() do{\
timeout(0);int k=getch();\
(k=='a'&&_0>1)?_0--:0;\
(k=='d'&&_0<W-2)?_0++:0;\
if(k==' '){F(i,MB){if(_1[i][0]==-1){_1[i][0]=H-2;_1[i][1]=_0;break;}}}\
if(k=='x')_5=1;\
}while(0)

#define UB() do{\
F(i,MB){if(_1[i][0]!=-1){_1[i][0]--;F(j,ME){\
if(_2[j][0]==_1[i][0]&&_2[j][1]==_1[i][1]){\
_2[j][0]=_2[j][1]=-1;_1[i][0]=-1;_4+=10;}}\
if(_1[i][0]<0)_1[i][0]=-1;}}\
}while(0)

#define UE() do{\
int e=0;\
F(i,ME){if(_2[i][1]+_3<=1||_2[i][1]+_3>=W-2){e=1;break;}}\
F(i,ME){if(_2[i][0]==-1)continue;\
if(e)_2[i][0]++;else _2[i][1]+=_3;\
if(_2[i][0]>=H-1)_5=1;}\
if(e)_3*=-1;\
}while(0)

int main(){initscr();noecho();curs_set(0);nodelay(stdscr,1);srand(time(0));I();
while(!_5){D();IN();UB();UE();usleep(100000);}
D();printw("\nX! S:%d\n",_4);printw("Key...");timeout(-1);getch();endwin();return 0;}
