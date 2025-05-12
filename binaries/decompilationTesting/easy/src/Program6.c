#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 40
#define HEIGHT 20
#define PADDLE_WIDTH 7
#define BRICK_ROWS 3
#define BRICK_COLS 10

int ballX = WIDTH / 2, ballY = HEIGHT - 5;
int ballDX = 1, ballDY = -1;
int paddleX = WIDTH / 2 - PADDLE_WIDTH / 2;
int bricks[BRICK_ROWS][BRICK_COLS];
int gameOver = 0;
int score = 0;

void setupBricks() {
    for (int i = 0; i < BRICK_ROWS; i++)
        for (int j = 0; j < BRICK_COLS; j++)
            bricks[i][j] = 1;
}

void draw() {
    clear();

    // Top border
    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("#");
        for (int x = 0; x < WIDTH; x++) {
            int brickDrawn = 0;

            // Bricks
            for (int i = 0; i < BRICK_ROWS; i++) {
                for (int j = 0; j < BRICK_COLS; j++) {
                    int bx = j * (WIDTH / BRICK_COLS);
                    int by = i + 1;
                    if (x >= bx && x < bx + (WIDTH / BRICK_COLS) && y == by && bricks[i][j]) {
                        printw("*");
                        brickDrawn = 1;
                        break;
                    }
                }
                if (brickDrawn) break;
            }

            // Ball
            if (!brickDrawn && x == ballX && y == ballY) printw("O");

            // Paddle
            else if (!brickDrawn && y == HEIGHT - 1 && x >= paddleX && x < paddleX + PADDLE_WIDTH)
                printw("=");

            else if (!brickDrawn) printw(" ");
        }
        printw("#\n");
    }

    // Bottom border
    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\nScore: %d\n", score);
    refresh();
}

void input() {
    timeout(0); // non-blocking
    int ch = getch();
    if (ch == 'a' && paddleX > 0) paddleX--;
    if (ch == 'd' && paddleX + PADDLE_WIDTH < WIDTH) paddleX++;
    if (ch == 'x') gameOver = 1;
}

void logic() {
    ballX += ballDX;
    ballY += ballDY;

    // Wall bounce
    if (ballX <= 0 || ballX >= WIDTH - 1) ballDX *= -1;
    if (ballY <= 0) ballDY *= -1;

    // Paddle bounce
    if (ballY == HEIGHT - 2 && ballX >= paddleX && ballX < paddleX + PADDLE_WIDTH)
        ballDY *= -1;

    // Brick collision
    for (int i = 0; i < BRICK_ROWS; i++) {
        for (int j = 0; j < BRICK_COLS; j++) {
            int bx = j * (WIDTH / BRICK_COLS);
            int by = i + 1;
            if (bricks[i][j] && ballY == by && ballX >= bx && ballX < bx + (WIDTH / BRICK_COLS)) {
                bricks[i][j] = 0;
                ballDY *= -1;
                score += 10;
            }
        }
    }

    if (ballY >= HEIGHT - 1) {
        gameOver = 1;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    setupBricks();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(50000); // 50ms
    }

    draw();
    printw("\nGame Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    return 0;
}
