#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define P 10
#define S(x) printw(x)
#define T timeout(0)
#define U usleep
#define R refresh
#define C clear()

int x=W/2,y=H-2,f=H-1,s=0,g=0,p[P][2];

void I(){srand(time(NULL));for(int i=0;i<P;i++){p[i][0]=rand()%(H-2);p[i][1]=rand()%W;}}
void D(){C;for(int Y=0;Y<H;Y++){for(int X=0;X<W;X++){int k=0;if(Y==f){S("^");continue;}for(int i=0;i<P;i++){if(p[i][0]==Y&&p[i][1]==X){S("-");k=1;break;}}if(!k&&Y==y&&X==x){S("@");k=1;}if(!k)S(" ");}S("\n");}printw("Score: %d\n",s);R();}
void In(){T;int c=getch();if(c=='a'&&x>0)x--;if(c=='d'&&x<W-1)x++;if(c=='w'&&y>0)y--;if(c=='x')g=1;}
void L(){f--;if(f<0)f=0;if(y==0){s++;x=W/2;y=H-2;f=H-1;for(int i=0;i<P;i++){p[i][0]=rand()%(H-2);p[i][1]=rand()%W;}}if(y>=f)g=1;}

int main(){initscr();noecho();curs_set(0);I();while(!g){D();In();L();U(120000);}D();printw("\n🔥 Burned! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
