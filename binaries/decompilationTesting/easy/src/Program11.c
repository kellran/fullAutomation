#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_FRUITS 5

int basketX = WIDTH / 2;
int fruits[MAX_FRUITS][2]; // [y][x]
int score = 0;
int misses = 0;
int gameOver = 0;

void init() {
    srand(time(NULL));
    for (int i = 0; i < MAX_FRUITS; i++) {
        fruits[i][0] = rand() % (HEIGHT / 2);
        fruits[i][1] = rand() % WIDTH;
    }
}

void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            if (y == HEIGHT - 1 && x >= basketX - 1 && x <= basketX + 1) {
                printw("U");
                printed = 1;
            } else {
                for (int i = 0; i < MAX_FRUITS; i++) {
                    if (fruits[i][0] == y && fruits[i][1] == x) {
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

    printw("Score: %d  Misses: %d\n", score, misses);
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();
    if (ch == 'a' && basketX > 1) basketX--;
    if (ch == 'd' && basketX < WIDTH - 2) basketX++;
    if (ch == 'x') gameOver = 1;
}

void logic() {
    for (int i = 0; i < MAX_FRUITS; i++) {
        fruits[i][0]++;

        if (fruits[i][0] == HEIGHT - 1) {
            if (fruits[i][1] >= basketX - 1 && fruits[i][1] <= basketX + 1) {
                score++;
            } else {
                misses++;
                if (misses >= 5) {
                    gameOver = 1;
                }
            }

            fruits[i][0] = 0;
            fruits[i][1] = rand() % WIDTH;
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
        usleep(100000); // 100ms tick
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
