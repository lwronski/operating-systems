//
// Created by lwronski on 12.05.19.
//
#include <sys/types.h>
#include <sys/ipc.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include "common.h"

#define TRUCK_NUMBER 4

key_t get_trucker_key(){
    char *home = getenv("HOME");
    if (home == NULL) exit_with_error("No find environment variable ", -1);

    key_t key = ftok(home, TRUCK_NUMBER);
    if ( key == -1 ) exit_with_error("Error with generating key",-1);
    return key;
}

void exit_with_error(char * error_message, int exit_code){
    perror(error_message);
    exit(exit_code);
}

int parse_arg_to_int(char *string) {

    char *endptr;
    long value = strtol(string, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || *string == '\0') {
        fprintf(stderr, "Error with cast argument to long \"%s\"\n", string);
        exit(0);
    }

    return (int) value;
}

void print_time(){
    struct timeval current_time;
    gettimeofday(&current_time, NULL);
    printf("[time]: %ld.%06ld", current_time.tv_sec, current_time.tv_usec);
}