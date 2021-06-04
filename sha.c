#include <ctype.h>
#include <fcntl.h>
#include <getopt.h>
#include <openssl/sha.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "config.h"

const struct option long_options[] = {
    {"file", required_argument, 0, 'f'},  {"type", required_argument, 0, 't'},
    {"block", required_argument, 0, 'b'}, {"help", no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},     {0, 0, 0, 0}};

const char *descript =
    "This program is used to calculate the hash signature of a file.";

const char *desc[] = {
    "The file to store contents",        "Choose a hash algorithm",
    "set the block size of hash buffer", "Show this help msg\n",
    "Show the version of this program",  ""};

int print_usage(int argc, char **argv);
void print_version();
void memDump(char *desc, void *addr, int len, char *prefix);
unsigned long strtosize(const char *restrict str);

const size_t fdsize(int fd);

unsigned char *fdsha1(char *filename, long block_size);
unsigned char *fdsha224(char *filename, long block_size);
unsigned char *fdsha256(char *filename, long block_size);
unsigned char *fdsha384(char *filename, long block_size);
unsigned char *fdsha512(char *filename, long block_size);

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "error: no args recived!\n");
        print_usage(argc, argv);
        return -1;
    }

    char *file_name = NULL;
    char *hash_algo = NULL;
    unsigned long block_size = 1024;

    int long_index = 0, opt = 0;
    while ((opt = getopt_long(argc, argv, "f:t:b:hv0", long_options,
                              &long_index)) != -1) {
        switch (opt) {
            case 'f':
                file_name = optarg;
                break;
            case 't':
                hash_algo = optarg;
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

    unsigned char *res = 0;
    int sha_len = 0;

    if (strncmp(hash_algo, "sha1", 4) == 0) {
        res = fdsha1(file_name, block_size);
        sha_len = SHA_DIGEST_LENGTH;
    }
    else if (strncmp(hash_algo, "sha224", 4) == 0) {
        res = fdsha224(file_name, block_size);
        sha_len = SHA224_DIGEST_LENGTH;
    }
    else if (strncmp(hash_algo, "sha256", 4) == 0) {
        res = fdsha256(file_name, block_size);
        sha_len = SHA256_DIGEST_LENGTH;
    }
    else if (strncmp(hash_algo, "sha384", 4) == 0) {
        res = fdsha384(file_name, block_size);
        sha_len = SHA384_DIGEST_LENGTH;
    }
    else if (strncmp(hash_algo, "sha512", 4) == 0) {
        res = fdsha512(file_name, block_size);
        sha_len = SHA512_DIGEST_LENGTH;
    }
    else {
        printf("sha algorithm not supported: %s\n", hash_algo);
        exit(-1);
    }

    for (int i = 0; i < sha_len; i++) {
        printf("%02x", res[i]);
    }
    printf("\n");

    free(res);

    return 0;
}

unsigned char *fdsha1(char *filename, long block_size)
{
    int fd = open(filename, O_RDONLY, 0666);

    long file_size = fdsize(fd);

    if (file_size == 0) {
        printf("0 byte got.\n");
        close(fd);
        return 0;
    }

    SHA_CTX sha_context;
    if (!SHA1_Init(&sha_context)) {
        perror("SHA1_Init failed!");
        exit(-1);
    }

    unsigned char *sha =
        (unsigned char *)malloc(sizeof(char) * SHA_DIGEST_LENGTH);

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    long sign = 0, diff = 0;
    unsigned long read_size = 0;
    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        read_size = block_size + (diff & sign);

        read(fd, buffer, read_size);

        if (!SHA1_Update(&sha_context, buffer, read_size)) {
            perror("SHA1_Update failed!");
            exit(-1);
        }

        file_size -= read_size;
    }
    close(fd);

    if (!SHA1_Final(sha, &sha_context)) {
        perror("SHA1_Final Failed!");
        exit(-1);
    }

    return sha;
}

unsigned char *fdsha224(char *filename, long block_size)
{
    int fd = open(filename, O_RDONLY, 0666);

    long file_size = fdsize(fd);

    if (file_size == 0) {
        printf("0 byte got.\n");
        close(fd);
        return 0;
    }

    SHA256_CTX sha_context;
    if (!SHA224_Init(&sha_context)) {
        perror("SHA1_Init failed!");
        exit(-1);
    }

    unsigned char *sha =
        (unsigned char *)malloc(sizeof(char) * SHA224_DIGEST_LENGTH);

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    long sign = 0, diff = 0;
    unsigned long read_size = 0;
    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        read_size = block_size + (diff & sign);

        read(fd, buffer, read_size);

        if (!SHA224_Update(&sha_context, buffer, read_size)) {
            perror("SHA1_Update failed!");
            exit(-1);
        }

        file_size -= read_size;
    }
    close(fd);

    if (!SHA224_Final(sha, &sha_context)) {
        perror("SHA1_Final Failed!");
        exit(-1);
    }

    return sha;
}
unsigned char *fdsha256(char *filename, long block_size)
{
    int fd = open(filename, O_RDONLY, 0666);

    long file_size = fdsize(fd);

    if (file_size == 0) {
        printf("0 byte got.\n");
        close(fd);
        return 0;
    }

    SHA256_CTX sha_context;
    if (!SHA256_Init(&sha_context)) {
        perror("SHA1_Init failed!");
        exit(-1);
    }

    unsigned char *sha =
        (unsigned char *)malloc(sizeof(char) * SHA256_DIGEST_LENGTH);

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    long sign = 0, diff = 0;
    unsigned long read_size = 0;
    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        read_size = block_size + (diff & sign);

        read(fd, buffer, read_size);

        if (!SHA256_Update(&sha_context, buffer, read_size)) {
            perror("SHA1_Update failed!");
            exit(-1);
        }

        file_size -= read_size;
    }
    close(fd);

    if (!SHA256_Final(sha, &sha_context)) {
        perror("SHA1_Final Failed!");
        exit(-1);
    }

    return sha;
}
unsigned char *fdsha384(char *filename, long block_size)
{
    int fd = open(filename, O_RDONLY, 0666);

    long file_size = fdsize(fd);

    if (file_size == 0) {
        printf("0 byte got.\n");
        close(fd);
        return 0;
    }

    SHA512_CTX sha_context;
    if (!SHA384_Init(&sha_context)) {
        perror("SHA1_Init failed!");
        exit(-1);
    }

    unsigned char *sha =
        (unsigned char *)malloc(sizeof(char) * SHA384_DIGEST_LENGTH);

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    long sign = 0, diff = 0;
    unsigned long read_size = 0;
    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        read_size = block_size + (diff & sign);

        read(fd, buffer, read_size);

        if (!SHA384_Update(&sha_context, buffer, read_size)) {
            perror("SHA1_Update failed!");
            exit(-1);
        }

        file_size -= read_size;
    }
    close(fd);

    if (!SHA384_Final(sha, &sha_context)) {
        perror("SHA1_Final Failed!");
        exit(-1);
    }

    return sha;
}
unsigned char *fdsha512(char *filename, long block_size)
{
    int fd = open(filename, O_RDONLY, 0666);

    long file_size = fdsize(fd);

    if (file_size == 0) {
        printf("0 byte got.\n");
        close(fd);
        return 0;
    }

    SHA512_CTX sha_context;
    if (!SHA512_Init(&sha_context)) {
        perror("SHA1_Init failed!");
        exit(-1);
    }

    unsigned char *sha =
        (unsigned char *)malloc(sizeof(char) * SHA512_DIGEST_LENGTH);

    char *buffer = (char *)malloc(sizeof(char) * block_size);

    long sign = 0, diff = 0;
    unsigned long read_size = 0;
    while (file_size != 0) {
        diff = file_size - block_size;
        sign = diff >> (sizeof(long) * 8 - 1);

        read_size = block_size + (diff & sign);

        read(fd, buffer, read_size);

        if (!SHA512_Update(&sha_context, buffer, read_size)) {
            perror("SHA1_Update failed!");
            exit(-1);
        }

        file_size -= read_size;
    }
    close(fd);

    if (!SHA512_Final(sha, &sha_context)) {
        perror("SHA1_Final Failed!");
        exit(-1);
    }

    return sha;
}

const size_t fdsize(int fd)
{
    off_t offset = lseek(fd, 0, SEEK_END);
    lseek(fd, 0, SEEK_SET);
    return offset;
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

