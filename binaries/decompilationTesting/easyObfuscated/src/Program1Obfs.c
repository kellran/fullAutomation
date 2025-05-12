#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 30
#define H 20
#define L 100

#define F(x,y) for(int x=0;x<y;x++)
#define EQ(a,b) ((a)==(b))

int _0[L],_1[L],_2=3,_3='d',_4,_5,_6=0,_7=0;

#define I() do{\
F(i,_2){_0[i]=W/2-i;_1[i]=H/2;}\
srand(time(0));\
_4=rand()%(W-2)+1;_5=rand()%(H-2)+1;\
}while(0)

#define D() do{\
clear();\
F(i,W+2)printw("#");printw("\n");\
F(y,H){printw("#");F(x,W){int p=0;\
if(x==_4&&y==_5){printw("F");continue;}\
F(i,_2){if(_0[i]==x&&_1[i]==y){printw(i==0?"O":"o");p=1;break;}}\
if(!p)printw(" ");}printw("#\n");}\
F(i,W+2)printw("#");printw("\nS:%d\n",_7);refresh();\
}while(0)

#define IN() do{timeout(0);int k=getch();\
((k=='w'||k=='s'||k=='a'||k=='d')&&abs(k-_3)!=21)?_3=k:0;\
(k=='x')?(_6=1):0;\
}while(0)

#define LGC() do{\
for(int i=_2-1;i>0;i--){_0[i]=_0[i-1];_1[i]=_1[i-1];}\
switch(_3){case 'w':_1[0]--;break;case 's':_1[0]++;break;\
case 'a':_0[0]--;break;case 'd':_0[0]++;break;}\
if(_0[0]<0||_0[0]>=W||_1[0]<0||_1[0]>=H)_6=1;\
F(i,_2)if(i&&_0[0]==_0[i]&&_1[0]==_1[i])_6=1;\
if(_0[0]==_4&&_1[0]==_5){_2++;_7+=10;_4=rand()%(W-2)+1;_5=rand()%(H-2)+1;}\
}while(0)

int main(){initscr();noecho();curs_set(0);I();
while(!_6){D();IN();LGC();usleep(100000);}
D();printw("\nX! S:%d\n",_7);printw("Key...");timeout(-1);getch();endwin();return 0;}
