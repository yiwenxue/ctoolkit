#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

#define file_limit 64

const struct option long_options[] = {{"files", required_argument, 0, 'f'},
                                      {"bsize", required_argument, 0, 'b'},
                                      {"help", no_argument, 0, 'h'},
                                      {"version", no_argument, 0, 'v'},
                                      {0, 0, 0, 0}};

const char *descript = "This program is an simple text editor.";

const char *desc[] = {"The text files to edit", "set the editor's buffer size",
                      "Show this help msg\n",
                      "Show the version of this program", ""};

unsigned long strtosize(const char *restrict str);
int print_usage(int argc, char **argv);
void print_version();
const size_t fdsize(int fd);

static int current_file = -1;
char *file_list[file_limit] = {NULL};
int fd[file_limit] = {-1};
unsigned long file_size[file_limit] = {-1};

void int_handle(int sig)
{
    printf("interrupt catched: current_id: %d, current_fd: %d\n", current_file,
           fd[current_file]);
    if (current_file != -1 && fd[current_file] != -1) {
        close(fd[current_file]);
        fd[current_file] = -1;
    }
}

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "error: no args recived!\n");
        print_usage(argc, argv);
        return -1;
    }

    int file_count = 0;
    char *buffer;

    unsigned long buffer_size = 1 << 10;

    signal(SIGINT, int_handle);

    int long_index = 0, opt = 0;
    while ((opt = getopt_long(argc, argv, "f:b:hv0", long_options,
                              &long_index)) != -1) {
        switch (opt) {
            case 'f':
                file_list[0] = optarg;
                for (file_count = 1; file_count < file_limit && optind < argc &&
                                     *argv[optind] != '-';
                     optind++) {
                    file_list[file_count++] = argv[optind];
                }
                break;
            case 'b':
                buffer_size = strtosize(optarg);
                break;
            case 'h':
                print_usage(argc, argv);
                return 0;
            case 'v':
                print_version();
                return 0;
            default:
                printf("Unexcepted input\n");
                print_usage(argc, argv);
                return -1;
        }
    }

    if (file_count == 0 || optind < argc) {
        for (; file_count < file_limit && optind < argc && *argv[optind] != '-';
             optind++) {
            file_list[file_count++] = argv[optind];
        }
    }

    if (file_count == 0) goto ext;

    buffer = (char *)malloc(sizeof(char) * buffer_size);

    for (int i = 0; i < file_count; i++) {
        fd[i] = open(file_list[i], O_RDWR | O_CREAT, 0666);
        if (fd[i] == -1) {
            perror("failed to open file descriptor.");
            goto ext;
        }
        file_size[i] = fdsize(fd[i]);
        printf("fd[%d] = %d, size: %ld\n", i, fd[i], file_size[i]);
    }

    unsigned long read_size = 0;
    unsigned long remain_size = 0;

    printf("sizeof buffer: %ld\n", buffer_size);

    /* current_file = 0; */
    /* remain_size = file_size[current_file]; */
    /* while (fd[current_file] != -1 && remain_size > 0) { */
    /*     read_size = buffer_size > remain_size ? remain_size : buffer_size; */
    /*     read_size = read(fd[current_file], buffer, read_size); */
    /*     remain_size -= read_size; */
    /*     printf("%s", buffer); */
    /* } */

    for (current_file = 0; current_file < file_count; current_file++) {
        lseek(fd[current_file], 0, SEEK_SET);
        remain_size = file_size[current_file];
        printf("sizeof %s: %ld\n", file_list[current_file],
               file_size[current_file]);
        while (fd[current_file] != -1 && remain_size > 0) {
            lseek(fd[current_file], 0, SEEK_SET);
            read_size = buffer_size > remain_size ? remain_size : buffer_size;
            /* remain_size -= read_size; */

            write(STDOUT_FILENO, "\033c", 2);
            /* printf("sizeof buffer: %ld\n", read_size); */

            read_size = read(fd[current_file], buffer, read_size);

            /* printf("read: %ld\tremain: %ld\n", read_size, remain_size); */

            write(STDOUT_FILENO, buffer, read_size);

            usleep(32e4);
        }
    }

    if (file_count) {
        printf("\nedited: ");
        for (int i = 0; i < file_count; ++i) {
            printf("%s ", file_list[i]);
        }
        printf("\b\n");
    }

ext:
    if (buffer != NULL) free(buffer);
    for (int i = 0; i < file_count; i++) {
        if (fd[i] != -1) close(fd[i]);
    }

    return 0;
}

int print_usage(int argc, char **argv)
{
    printf(
        "Usage: \n %s <opt> <arg>\n"
        "%s\n",
        argv[0], descript);
    int i = 0;
    while (1) {
        if (long_options[i].name != NULL)
            printf("\t-%c, --%-15s    %s\n", long_options[i].val,
                   long_options[i].name, desc[i]);
        else
            break;
        i++;
    }
    return 0;
}

void print_version()
{
    fprintf(stdout, "VERSION: %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR,
            VERSION_PATCH);
}

unsigned long strtosize(const char *restrict str)
{
    unsigned long file_size = 0;
    int arg_len = strlen(str);
    if (arg_len > 20) {
        printf("the input size is not supported %s, please use B,K,M,G\n", str);
    }

    char *pEnd, dim;
    if (str[arg_len - 1] < '0' || str[arg_len - 1] > '9') {
        dim = str[arg_len - 1];
    }
    else
        dim = 'B';
    dim = tolower(dim);

    file_size = strtol(str, &pEnd, 10);

    switch (dim) {
        case 'b':
            break;
        case 'k':
            file_size <<= 10;
            break;
        case 'm':
            file_size <<= 20;
            break;
        case 'g':
            file_size <<= 30;
            break;
        default:
            printf("Wrong units: %c\n", dim);
            return -1;
    }
    return file_size;
}

const size_t fdsize(int fd)
{
    off_t offset = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return offset;
}
