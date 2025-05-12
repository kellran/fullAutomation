#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define W 40
#define H 20
#define B 5
#define E 10
#define S(x) printw(x)
#define T timeout(0)
#define U usleep
#define R refresh
#define C clear

int p=W/2,b[B][2],e[E][2],s=0,g=0;

void i(){for(int i=0;i<B;i++)b[i][0]=-1;for(int i=0;i<E;i++){e[i][0]=rand()%5;e[i][1]=rand()%W;}}
void d(){C();for(int y=0;y<H;y++){for(int x=0;x<W;x++){int P=0;if(y==H-1&&x==p){S("A");P=1;}else{for(int j=0;j<E;j++){if(e[j][0]==y&&e[j][1]==x){S("M");P=1;break;}}}if(!P){for(int j=0;j<B;j++){if(b[j][0]==y&&b[j][1]==x){S("|");P=1;break;}}}if(!P)S(" ");}S("\n");}printw("Score: %d\n",s);R();}
void n(){T;int c=getch();if(c=='a'&&p>0)p--;if(c=='d'&&p<W-1)p++;if(c==' '){for(int i=0;i<B;i++){if(b[i][0]==-1){b[i][0]=H-2;b[i][1]=p;break;}}}if(c=='x')g=1;}
void ub(){for(int i=0;i<B;i++){if(b[i][0]!=-1){b[i][0]--;for(int j=0;j<E;j++){if(e[j][0]==b[i][0]&&e[j][1]==b[i][1]){e[j][0]=0;e[j][1]=rand()%W;b[i][0]=-1;s+=10;}}if(b[i][0]<0)b[i][0]=-1;}}}
void ue(){for(int i=0;i<E;i++){e[i][0]++;if(e[i][0]>=H-1)g=1;}}

int main(){initscr();noecho();curs_set(0);nodelay(stdscr,1);srand(time(NULL));i();while(!g){d();n();ub();ue();U(100000);}d();printw("\nGame Over! Final Score: %d\n",s);printw("Press any key to exit...");T;getch();endwin();return 0;}
