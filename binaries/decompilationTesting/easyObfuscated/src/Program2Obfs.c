#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

#define W 20
#define H 10
#define FOR(x,y) for(int x=0;x<y;x++)
#define EQ(a,b) ((a)==(b))

char _0[H][W+1]={
"###################",
"#@      #        #",
"#  #### # ###### #",
"#     # #      # #",
"##### # ###### # #",
"#     #    #   # #",
"# ######## # ### #",
"#        # #     #",
"# ######## #####X#",
"###################"};

int _1=1,_2=1,_3=0;

#define D() do{clear();FOR(i,H)printw("%s\n",_0[i]);printw("Use WASD. Find X\n");refresh();}while(0)

#define I() do{\
timeout(0);int k=getch(),nx=_1,ny=_2;\
k=='w'?(ny--):(k=='s'?(ny++):(k=='a'?(nx--):(k=='d'?(nx++):0)));\
char n=_0[ny][nx];\
if(n==' '||n=='X'){_0[_2][_1]=' ';_1=nx;_2=ny;_0[_2][_1]='@';if(n=='X')_3=1;}\
}while(0)

int main(){initscr();noecho();curs_set(0);D();
while(!_3){I();D();usleep(80000);}
D();printw("\nX!\nPress key...");timeout(-1);getch();endwin();return 0;}
