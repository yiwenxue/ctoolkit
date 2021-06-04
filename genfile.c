#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

char *string =
    "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789,.-#'?!"
    "\n\n\n";

#define BUFFER_SIZE 1024

const struct option long_options[] = {
    {"files", required_argument, 0, 'f'}, {"size", required_argument, 0, 's'},
    {"block", required_argument, 0, 'b'}, {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},     {0, 0, 0, 0}};

const char *descript =
    "This program is used to generate huge file with random contents.";

const char *desc[] = {"The file to store content",
                      "set the size of the file that you want to generate",
                      "set the block size or the buffer size",
                      "Show this help msg\n",
                      "Show the version of this program",
                      ""};

int print_usage(int argc, char **argv);
void print_version();
void memDump(char *desc, void *addr, int len, char *prefix);

unsigned long strtosize(const char *restrict str);

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "error: no args recived!\n");
        print_usage(argc, argv);
        return -1;
    }

    char *file_name = NULL;
    unsigned long file_size = 0;
    unsigned long block_size = 1024;

    int long_index = 0, opt = 0;
    while ((opt = getopt_long(argc, argv, "f:s:b:hv0", long_options,
                              &long_index)) != -1) {
        switch (opt) {
            case 'f':
                file_name = optarg;
                break;
            case 's':
                file_size = strtosize(optarg);
                break;
            case 'b':
                block_size = strtosize(optarg);
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

    printf("file size: %ld\n", file_size);

    if (file_size > ((long)1 << 36)) {
        printf("Size tooo large: %ldG, limit 64G\n", file_size >> 30);
        return -1;
    }

    int fd = open(file_name, O_WRONLY | O_CREAT, 0666);

    if (file_size == 0) {
        printf("0 byte wrote.\n");
        close(fd);
        return 0;
    }

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    const int mySeed = clock();
    srand(mySeed);
    size_t stringLen = strlen(string);
    char *randomString = NULL;
    unsigned long write_size = 0;

    long sign = 0, diff = 0;

    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        write_size = block_size + (diff & sign);

        for (size_t i = 0; i < write_size; i++) {
            buffer[i] = string[rand() % stringLen];
        }

        write(fd, buffer, write_size);

        file_size -= write_size;
    }

    close(fd);

    return 0;
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

void memDump(char *desc, void *addr, int len, char *prefix)
{
    int i;
    unsigned char buff[17];
    unsigned char *ptr = (unsigned char *)addr;

    if (desc != NULL) printf("%s%s:\n", prefix, desc);
    for (i = 0; i < len; i++) {
        if ((i % 16) == 0) {
            if (i != 0) printf(" | %s\n", buff);
            printf("%s %04x |", prefix, i);
        }
        printf(" %02x", ptr[i]);
        if ((ptr[i] < 0x20) || (ptr[i] > 0x7e)) {
            buff[i % 16] = '.';
        }
        else {
            buff[i % 16] = ptr[i];
        }
        buff[(i % 16) + 1] = '\0';
    }
    while ((i % 16) != 0) {
        printf("   ");
        i++;
    }
    printf(" | %s\n", buff);
}
