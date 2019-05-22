#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>
#include <zconf.h>
#include "common.h"

int main(int argc, char ** argv) {

    if (argc < 3) {
        printf("Not enough arguments, expected 1 argument \n"
               "\t 'number of loader' 'max_weight_load' ");
        exit(0);
    }

    srand(time(NULL));
    int number_of_loader = parse_arg_to_int(argv[1]);
    int max_number_load = parse_arg_to_int(argv[2]);
    int i;
    char buff[20];

    for( i = 0; i < number_of_loader; i++ ) {
        int pid = fork();
        sprintf(buff, "%d", max_number_load);

        if ( pid == 0 ){
            execl("./loader", "loader",  buff , "10", NULL);
        }
    }

    return 0;
}