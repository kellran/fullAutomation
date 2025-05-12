#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define R 5
#define C clear
#define S(x) printw(x)
#define T timeout(0)
#define U usleep
#define F refresh

int pX,g=0,s=0,r[R][2];

void I(){pX=W/2;srand(time(NULL));for(int i=0;i<R;i++){r[i][0]=rand()%H;r[i][1]=rand()%W;}}
void D(){C();for(int y=0;y<H;y++){for(int x=0;x<W;x++){int f=0;if(y==H-1&&x==pX){S("A");f=1;}else{for(int i=0;i<R;i++){if(r[i][0]==y&&r[i][1]==x){S("*");f=1;break;}}}if(!f)S(" ");}S("\n");}printw("Score: %d\n",s);F();}
void In(){T;int c=getch();if(c=='a'&&pX>0)pX--;if(c=='d'&&pX<W-1)pX++;if(c=='x')g=1;}
void L(){for(int i=0;i<R;i++){r[i][0]++;if(r[i][0]>=H){r[i][0]=0;r[i][1]=rand()%W;s++;}if(r[i][0]==H-1&&r[i][1]==pX)g=1;}}

int main(){initscr();noecho();curs_set(FALSE);I();while(!g){D();In();L();U(80000);}D();printw("\nGame Over! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
