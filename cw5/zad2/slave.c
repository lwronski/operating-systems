//
// Created by lwronski on 06.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <fcntl.h>
#include <zconf.h>
#include <time.h>
#include "file_service.h"

int main(int argc, char **argv) {

    srand((unsigned int) time(NULL));

    char *path;
    int N, i;

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t<path> <N> \n"
        );
        exit(0);
    }

    if (argc < 3) {
        exit_with_error("You don't specify all required arguments \n", 1);
    }

    path = argv[1];
    N = (int) parse_arg_to_long(argv[2]);

    FILE * fifo = open_file(path, "w");
    FILE * output ;

    for ( i = 0; i < N; i++) {

        char buff[32];

        output = popen("date", "r");
        fgets(buff, 32, output);


        char out[PIPE_BUF];
        sprintf(out, "PID: %d, %s \n", getpid(), out);
        fputs(buff, fifo);
        fflush(fifo);
        sleep((unsigned int) (rand() % 4 + 2));
    }

    fclose(fifo);

}