#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>

#define WIDTH 40
#define HEIGHT 20
#define PADDLE_HEIGHT 4

int paddleY;
int ballX, ballY;
int ballDX = 1, ballDY = 1;
int score = 0;
int gameOver = 0;

void init() {
    paddleY = HEIGHT / 2 - PADDLE_HEIGHT / 2;
    ballX = WIDTH / 2;
    ballY = HEIGHT / 2;
    srand(time(NULL));
    ballDX = (rand() % 2) ? 1 : -1;
    ballDY = (rand() % 2) ? 1 : -1;
}

void draw() {
    clear();
    // Top border
    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("#");
        for (int x = 0; x < WIDTH; x++) {
            if (x == 2 && y >= paddleY && y < paddleY + PADDLE_HEIGHT) {
                printw("|"); // Paddle
            } else if (x == ballX && y == ballY) {
                printw("O"); // Ball
            } else {
                printw(" ");
            }
        }
        printw("#\n");
    }

    // Bottom border
    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    printw("Score: %d\n", score);
    refresh();
}

void input() {
    timeout(0);
    int ch = getch();
    if (ch == 'w' && paddleY > 0) paddleY--;
    if (ch == 's' && paddleY + PADDLE_HEIGHT < HEIGHT) paddleY++;
    if (ch == 'x') gameOver = 1;
}

void logic() {
    ballX += ballDX;
    ballY += ballDY;

    // Ball collision with top/bottom
    if (ballY <= 0 || ballY >= HEIGHT - 1) {
        ballDY *= -1;
    }

    // Ball collision with paddle
    if (ballX == 3) {
        if (ballY >= paddleY && ballY < paddleY + PADDLE_HEIGHT) {
            ballDX *= -1;
            score++;
        }
    }

    // Ball missed paddle
    if (ballX <= 0) {
        gameOver = 1;
    }

    // Ball collision with right wall
    if (ballX >= WIDTH - 1) {
        ballDX *= -1;
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
        usleep(60000); // ~60ms frame
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
