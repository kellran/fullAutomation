#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define F 5
#define S(x) printw(x)
#define T timeout(0)
#define U usleep
#define R refresh
#define C clear()

int bX=W/2,f[F][2],s=0,m=0,g=0;

void I(){srand(time(NULL));for(int i=0;i<F;i++){f[i][0]=rand()%(H/2);f[i][1]=rand()%W;}}
void D(){C;for(int y=0;y<H;y++){for(int x=0;x<W;x++){int P=0;if(y==H-1&&x>=bX-1&&x<=bX+1){S("U");P=1;}else{for(int i=0;i<F;i++){if(f[i][0]==y&&f[i][1]==x){S("*");P=1;break;}}}if(!P)S(" ");}S("\n");}printw("Score: %d  Misses: %d\n",s,m);R();}
void In(){T;int c=getch();if(c=='a'&&bX>1)bX--;if(c=='d'&&bX<W-2)bX++;if(c=='x')g=1;}
void L(){for(int i=0;i<F;i++){f[i][0]++;if(f[i][0]==H-1){if(f[i][1]>=bX-1&&f[i][1]<=bX+1)s++;else{m++;if(m>=5)g=1;}f[i][0]=0;f[i][1]=rand()%W;}}}

int main(){initscr();noecho();curs_set(0);I();while(!g){D();In();L();U(100000);}D();printw("\nGame Over! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
