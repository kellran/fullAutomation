#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define L 4
#define C 3
#define S(x) printw(x)
#define U usleep
#define T timeout(0)
#define R refresh
#define CL clear

typedef struct{int y,x[C],d;}L_;L_ l[L];int fX=W/2,fY=H-1,s=0,g=0;

void I(){srand(time(NULL));for(int i=0;i<L;i++){l[i].y=2+i*3;l[i].d=(i%2)?-1:1;for(int j=0;j<C;j++)l[i].x[j]=rand()%W;}}
void D(){CL();for(int y=0;y<H;y++){for(int x=0;x<W;x++){int P=0;if(y==0){S("~");continue;}for(int i=0;i<L;i++){if(y==l[i].y){for(int j=0;j<C;j++){if(x==l[i].x[j]){S("=");P=1;break;}}}}if(!P&&y==fY&&x==fX){S("@");P=1;}if(!P)S(" ");}S("\n");}printw("Score: %d\n",s);R();}
void In(){T;int c=getch();if(c=='w'&&fY>0)fY--;if(c=='s'&&fY<H-1)fY++;if(c=='x')g=1;}
void UC(){for(int i=0;i<L;i++)for(int j=0;j<C;j++){l[i].x[j]+=l[i].d;if(l[i].x[j]<0)l[i].x[j]=W-1;if(l[i].x[j]>=W)l[i].x[j]=0;}}
void CH(){for(int i=0;i<L;i++)if(fY==l[i].y)for(int j=0;j<C;j++)if(fX==l[i].x[j]){g=1;return;}if(fY==0){s++;fY=H-1;fX=W/2;}}

int main(){initscr();noecho();curs_set(0);I();while(!g){D();In();UC();CH();U(120000);}D();printw("\n🚗💀 Splat! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
