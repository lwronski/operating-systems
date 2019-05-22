//
// Created by lwronski on 23.03.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>
#include <values.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include "file_service.h"

void generate_record(char *buff, int record_length) {

    int i;

    for (i = 0; i < record_length; i++) {
        buff[i] = (char) ('a' + (rand() % 25));
    }

    buff[record_length-1] = '\0';
}

int main(int argc, char **argv) {

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "Possible operations: \n "
               "\t<file> <pmin> <pmax> <bytes>\n"
        );
        exit(0);
    }

    if (argc < 5) {
        fprintf(stderr, "You don't specify all required arguments");
        exit(1);
    }

    char *file_name = argv[1];
    long pmin = parse_arg_to_long(argv[2]);
    long pmax = parse_arg_to_long(argv[3]);
    long number_bytes = parse_arg_to_long(argv[4]);

    if (pmin <= 0 || pmax <= 0) {
        fprintf(stderr, "Value p must be positive");
        exit(1);
    }

    if (number_bytes <= 0) {
        fprintf(stderr, "Value bytes must be positive");
        exit(1);
    }

    if (pmin > pmax) {
        fprintf(stderr, "Pmin must be lower than pmax");
        exit(1);
    }

    int interval = (int) (rand() % (pmax - pmin + 1) + pmin);


    FILE* file = open_file(file_name, "a");

    struct timeval current_real_time;
    int i;

    for ( i = 0; i < 3; i++ ){

        gettimeofday(&current_real_time, 0);
        char time_char[100];
        strftime(time_char, 100, "%Y-%m-%d_%H-%M-%S", localtime(&current_real_time.tv_sec));

        char * random_bytes = malloc(number_bytes * sizeof(char));
        generate_record(random_bytes, (int) number_bytes);

        char * buff = malloc(number_bytes + 100);
        sprintf(buff, "%d %d %s %s \n", getpid(), interval, time_char, random_bytes);
        printf("%s \n",buff);
        fwrite(buff, sizeof(char), strlen(buff), file);

        free(random_bytes);
        free(buff);
        sleep((unsigned int) interval);
    }


    fclose(file);
    return 0;

}