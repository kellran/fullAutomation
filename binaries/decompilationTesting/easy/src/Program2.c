#include <stdio.h>
#include <ncurses.h>
#include <unistd.h>

#define WIDTH 20
#define HEIGHT 10

char maze[HEIGHT][WIDTH + 1] = {
    "###################",
    "#@      #        #",
    "#  #### # ###### #",
    "#     # #      # #",
    "##### # ###### # #",
    "#     #    #   # #",
    "# ######## # ### #",
    "#        # #     #",
    "# ######## #####X#",
    "###################"
};

int px = 1, py = 1; // Player start
int gameOver = 0;

void draw() {
    clear();
    for (int i = 0; i < HEIGHT; i++) {
        printw("%s\n", maze[i]);
    }
    printw("Use WASD to move. Reach the X!\n");
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();

    int nx = px, ny = py;

    if (ch == 'w') ny--;
    else if (ch == 's') ny++;
    else if (ch == 'a') nx--;
    else if (ch == 'd') nx++;

    char next = maze[ny][nx];

    if (next == ' ' || next == 'X') {
        maze[py][px] = ' ';
        px = nx;
        py = ny;
        maze[py][px] = '@';

        if (next == 'X') gameOver = 1;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    draw();

    while (!gameOver) {
        input();
        draw();
        usleep(80000); // smooth refresh
    }

    draw();
    printw("\n🎉 You escaped the maze! 🎉\n");
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
