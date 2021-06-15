#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioccom.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

void handle_int(int sig_num)
{
    fflush(stdout);
    printf("\nInterrupted by signal SIGINT.");
    exit(sig_num);
}

#define progress_bar_len_max 512

int progress(float prog, FILE *file, const char *_fmt, ...)
{
    if (prog > 100.0) {
        prog = 100.0;
    }
    va_list ap;
    va_start(ap, _fmt);

    char progress_buffer[progress_bar_len_max] = {0};

    struct winsize w;
    ioctl(0, TIOCGWINSZ, &w);
    vsnprintf(progress_buffer, progress_bar_len_max, _fmt, ap);

    int total =
        w.ws_col > progress_bar_len_max ? progress_bar_len_max : w.ws_col;

    int len = strnlen(progress_buffer, progress_bar_len_max);
    int tail = 5;
    int fin = (total - len - 4 - tail) * (prog / 100.0);
    int rest = (total - len - 4 - tail) - fin;

    progress_buffer[len] = ' ';
    progress_buffer[len + 1] = '[';

    for (int i = 0; i < fin; i++) {
        progress_buffer[len + 2 + i] = '#';
    }
    for (int i = 0; i < rest; i++) {
        progress_buffer[len + 2 + i + fin] = '.';
    }

    snprintf(progress_buffer, progress_bar_len_max, "%s] %4.1f%c",
             progress_buffer, prog, '%');

    fwrite(progress_buffer, total, 1, file);
    fflush(file);
    va_end(ap);
    return 0;
}
int main(int argc, char *argv[])
{
    signal(SIGINT, handle_int);

    int colnum = 0;

    float val = 0.0;
    srand(clock());

    struct winsize w;
    val = 0;
    while (val - 100.0 < 0) {
        ioctl(0, TIOCGWINSZ, &w);
        if (colnum == w.ws_col) {
            printf("\r");
        }
        else
            printf("\r");
        colnum = w.ws_col;
        val += 1;
        progress(val, stdout, "%-10s", "task1");
        usleep(1e5);
    }
    printf("\n");
    val = 0;
    while (val - 100.0 < 0) {
        ioctl(0, TIOCGWINSZ, &w);
        if (colnum == w.ws_col) {
            printf("\r");
        }
        else
            printf("\r");
        colnum = w.ws_col;
        val += 1;
        progress(val, stdout, "%-10s", "task2");
        usleep(5e4);
    }
    printf("\n");
    val = 0;
    while (val - 100.0 < 0) {
        ioctl(0, TIOCGWINSZ, &w);
        if (colnum == w.ws_col) {
            printf("\r");
        }
        else
            printf("\r");
        colnum = w.ws_col;
        val += 1;
        progress(val, stdout, "%-10s", "task3");
        usleep(5e4);
    }
    printf("\n");
    val = 0;
    while (val - 100.0 < 0) {
        ioctl(0, TIOCGWINSZ, &w);
        if (colnum == w.ws_col) {
            printf("\r");
        }
        else
            printf("\r");
        colnum = w.ws_col;
        val += 1;
        progress(val, stdout, "%-10s", "task4");
        usleep(5e4);
    }
    printf("\n");
    return 0;
}
