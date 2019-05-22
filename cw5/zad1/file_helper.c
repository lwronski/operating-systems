//
// Created by lwronski on 05.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include "pipe.h"

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