#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define NUM_ROCKS 5

int playerX;
int rocks[NUM_ROCKS][2]; // [y][x]
int score = 0;
int gameOver = 0;

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
                printw("A"); // Player
                printed = 1;
            } else {
                for (int i = 0; i < NUM_ROCKS; i++) {
                    if (rocks[i][0] == y && rocks[i][1] == x) {
                        printw("*"); // Rock
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
    if (ch == 'a' && playerX > 0) playerX--;
    if (ch == 'd' && playerX < WIDTH - 1) playerX++;
    if (ch == 'x') gameOver = 1;
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

    return 0;
}
