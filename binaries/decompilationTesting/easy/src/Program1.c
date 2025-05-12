#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 30
#define HEIGHT 20
#define MAX_LEN 100

int snakeX[MAX_LEN], snakeY[MAX_LEN];
int length = 3;
int dir = 'd'; // 'w', 's', 'a', 'd'
int fruitX, fruitY;
int gameOver = 0;
int score = 0;

void init() {
    for (int i = 0; i < length; i++) {
        snakeX[i] = WIDTH / 2 - i;
        snakeY[i] = HEIGHT / 2;
    }

    srand(time(0));
    fruitX = rand() % (WIDTH - 2) + 1;
    fruitY = rand() % (HEIGHT - 2) + 1;
}

void draw() {
    clear();

    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("#");
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            if (x == fruitX && y == fruitY) {
                printw("F");
                continue;
            }

            for (int i = 0; i < length; i++) {
                if (snakeX[i] == x && snakeY[i] == y) {
                    printw(i == 0 ? "O" : "o");
                    printed = 1;
                    break;
                }
            }

            if (!printed) printw(" ");
        }
        printw("#\n");
    }

    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\nScore: %d\n", score);
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();
    if ((ch == 'w' || ch == 's' || ch == 'a' || ch == 'd') && abs(ch - dir) != 21)
        dir = ch;
    if (ch == 'x') gameOver = 1;
}

void logic() {
    // Move body
    for (int i = length - 1; i > 0; i--) {
        snakeX[i] = snakeX[i - 1];
        snakeY[i] = snakeY[i - 1];
    }

    // Head movement
    switch (dir) {
        case 'w': snakeY[0]--; break;
        case 's': snakeY[0]++; break;
        case 'a': snakeX[0]--; break;
        case 'd': snakeX[0]++; break;
    }

    // Collision with wall
    if (snakeX[0] < 0 || snakeX[0] >= WIDTH || snakeY[0] < 0 || snakeY[0] >= HEIGHT)
        gameOver = 1;

    // Collision with self
    for (int i = 1; i < length; i++) {
        if (snakeX[0] == snakeX[i] && snakeY[0] == snakeY[i]) {
            gameOver = 1;
            break;
        }
    }

    // Eat fruit
    if (snakeX[0] == fruitX && snakeY[0] == fruitY) {
        length++;
        score += 10;
        fruitX = rand() % (WIDTH - 2) + 1;
        fruitY = rand() % (HEIGHT - 2) + 1;
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
