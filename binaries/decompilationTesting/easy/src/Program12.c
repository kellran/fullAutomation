#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_PLATFORMS 10

int playerX = WIDTH / 2;
int playerY = HEIGHT - 2;
int fireLevel = HEIGHT - 1;
int score = 0;
int gameOver = 0;

int platforms[MAX_PLATFORMS][2]; // [y][x]

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
                printw("^"); // fire
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

    if (ch == 'a' && playerX > 0) playerX--;
    if (ch == 'd' && playerX < WIDTH - 1) playerX++;
    if (ch == 'w' && playerY > 0) playerY--;
    if (ch == 'x') gameOver = 1;
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

    return 0;
}
