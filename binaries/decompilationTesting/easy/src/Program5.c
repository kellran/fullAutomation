#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 40
#define HEIGHT 20
#define GAP 6
#define PIPE_WIDTH 3

int birdY = HEIGHT / 2;
int birdX = 5;
int velocity = 0;
int gravity = 1;
int score = 0;
int pipeX = WIDTH;
int gapY;
int gameOver = 0;

void init() {
    srand(time(0));
    gapY = rand() % (HEIGHT - GAP - 2) + 1;
}

void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 || x == WIDTH - 1) {
                printw("#");
            } else if (x == birdX && y == birdY) {
                printw("O");
            } else if (x >= pipeX && x < pipeX + PIPE_WIDTH &&
                       (y < gapY || y > gapY + GAP)) {
                printw("|");
            } else {
                printw(" ");
            }
        }
        printw("\n");
    }

    for (int i = 0; i < WIDTH; i++) printw("#");
    printw("\nScore: %d\n", score);
    refresh();
}

void input() {
    timeout(0); // non-blocking input
    int ch = getch();
    if (ch == ' ') {
        velocity = -3;
    } else if (ch == 'x') {
        gameOver = 1;
    }
}

void logic() {
    velocity += gravity;
    birdY += velocity;

    if (birdY <= 0 || birdY >= HEIGHT - 1) {
        gameOver = 1;
    }

    pipeX--;
    if (pipeX + PIPE_WIDTH < 0) {
        pipeX = WIDTH;
        gapY = rand() % (HEIGHT - GAP - 2) + 1;
        score++;
    }

    if (birdX >= pipeX && birdX < pipeX + PIPE_WIDTH &&
        (birdY < gapY || birdY > gapY + GAP)) {
        gameOver = 1;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);

    init();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(100000); // 100ms
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
