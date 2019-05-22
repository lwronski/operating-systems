//
// Created by lwronski on 24.03.19.
//

#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include"file_service.h"


void exit_with_error(const char *error, int exit_code) {
    perror(error);
    exit(exit_code);
}

FILE *open_file(char *file_name, char *mode) {

    FILE *file = fopen(file_name, mode);

    if (file == NULL) {
        fprintf(stderr, "Error with open file: %s\n", file_name);
        exit_with_error("", 0);
    }

    return file;
}

struct stat get_file_info(char *path) {

    struct stat file_stat;

    if (lstat(path, &file_stat) != 0) {
        fprintf(stderr, "Error with getting info status\n");
        perror(path);
        exit(0);
    }

    return file_stat;
}

long parse_arg_to_long(char *string) {
    char *endptr;

    long value = strtol(string, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || *string == '\0') {
        fprintf(stderr, "Error with cast argument to long \"%s\"\n", string);
        exit(0);
    }

    return value;
}

int get_number_of_line(char *file_name) {

    FILE *file = open_file(file_name, "r");
    int lines = 0;

    while (!feof(file)) {
        char ch = fgetc(file);
        if (ch == '\n') {
            lines++;
        }
    }

    fclose(file);
    return lines + 1;
}