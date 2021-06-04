#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "comments.h"
#include "config.h"

const struct option long_options[] = {{"files", required_argument, 0, 'f'},
                                      {"help", no_argument, 0, 'h'},
                                      {"version", no_argument, 0, 'v'},
                                      {0, 0, 0, 0}};

const char *descript =
    "This program is designed to extract the information in comments in c/c++ "
    "source files, "
    "and print the structed info in the json format.";

const char *desc[] = {"The input source files", "Show this help msg\n",
                      "Show the version of this program", ""};

int print_usage(int argc, char **argv);
void print_version();

int main(int argc, char **argv)
{
    if (argc == 1) {
        fprintf(stderr, "error: no args recived!\n");
        print_usage(argc, argv);
        return -1;
    }

    char *file_list[1024] = {NULL};
    int file_count = 0;

    int long_index = 0, opt = 0;
    while ((opt = getopt_long(argc, argv, "f:hv0", long_options,
                              &long_index)) != -1) {
        switch (opt) {
            case 'f':
                file_list[0] = optarg;
                for (file_count = 1; optind < argc && *argv[optind] != '-';
                     optind++) {
                    file_list[file_count++] = argv[optind];
                }
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

    if (file_count) {
        printf("file list: ");
        for (int i = 0; i < file_count; ++i) {
            printf("%s ", file_list[i]);
        }
        printf("\b\n");
    }

    {
        printf("HOME: %s\n", getenv("HOME"));
        printf("PATCH_PATH: %s\n", getenv("PATCH_PATH"));
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
