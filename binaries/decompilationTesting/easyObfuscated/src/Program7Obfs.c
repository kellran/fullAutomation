#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define P 4
#define C clear
#define T timeout(0)
#define R refresh
#define U usleep
#define S(x) printw(x)

int pY,bX,bY,bDX=1,bDY=1,s=0,g=0;

void I(){pY=H/2-P/2;bX=W/2;bY=H/2;srand(time(NULL));bDX=(rand()%2)?1:-1;bDY=(rand()%2)?1:-1;}
void D(){C();for(int i=0;i<W+2;i++)S("#");S("\n");for(int y=0;y<H;y++){S("#");for(int x=0;x<W;x++){if(x==2&&y>=pY&&y<pY+P)S("|");else if(x==bX&&y==bY)S("O");else S(" ");}S("#\n");}for(int i=0;i<W+2;i++)S("#");printw("\nScore: %d\n",s);R();}
void In(){T;int c=getch();if(c=='w'&&pY>0)pY--;if(c=='s'&&pY+P<H)pY++;if(c=='x')g=1;}
void L(){bX+=bDX;bY+=bDY;if(bY<=0||bY>=H-1)bDY*=-1;if(bX==3){if(bY>=pY&&bY<pY+P){bDX*=-1;s++;}}if(bX<=0)g=1;if(bX>=W-1)bDX*=-1;}

int main(){initscr();noecho();curs_set(FALSE);I();while(!g){D();In();L();U(60000);}D();printw("\nGame Over! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
