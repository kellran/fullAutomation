#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_BULLETS 5
#define MAX_ENEMIES 10

int playerX = WIDTH / 2;
int bullets[MAX_BULLETS][2]; // [y][x]
int enemies[MAX_ENEMIES][2]; // [y][x]
int score = 0;
int gameOver = 0;

void init() {
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i][0] = -1; // inactive

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i][0] = rand() % 5; // y
        enemies[i][1] = rand() % WIDTH; // x
    }
}

void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            // Player
            if (y == HEIGHT - 1 && x == playerX) {
                printw("A");
                continue;
            }

            // Enemies
            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i][0] == y && enemies[i][1] == x) {
                    printw("M");
                    printed = 1;
                    break;
                }
            }

            // Bullets
            if (!printed) {
                for (int b = 0; b < MAX_BULLETS; b++) {
                    if (bullets[b][0] == y && bullets[b][1] == x) {
                        printw("|");
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
    if (ch == ' ') {
        for (int i = 0; i < MAX_BULLETS; i++) {
            if (bullets[i][0] == -1) {
                bullets[i][0] = HEIGHT - 2;
                bullets[i][1] = playerX;
                break;
            }
        }
    }
    if (ch == 'x') gameOver = 1;
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i][0] != -1) {
            bullets[i][0]--;

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j][0] == bullets[i][0] &&
                    enemies[j][1] == bullets[i][1]) {
                    enemies[j][0] = 0;
                    enemies[j][1] = rand() % WIDTH;
                    bullets[i][0] = -1;
                    score += 10;
                }
            }

            if (bullets[i][0] < 0) bullets[i][0] = -1;
        }
    }
}

void updateEnemies() {
    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i][0]++;
        if (enemies[i][0] >= HEIGHT - 1) {
            gameOver = 1;
        }
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    srand(time(NULL));

    init();

    while (!gameOver) {
        draw();
        input();
        updateBullets();
        updateEnemies();
        usleep(100000); // ~100ms per frame
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
