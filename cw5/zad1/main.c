#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include "pipe.h"

int main(int argc, char **argv) {

    if ((argc == 1) || strcmp(argv[1], "-help") == 0) {
        printf("Arguments are: \n"
               "\t<path>\n");
        exit(0);
    }

    char * path = argv[1];

    execute_command(path);

    return 0;
}