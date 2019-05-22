//
// Created by lwronski on 27.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>

void exit_error(char * error_message, int exit_code){
    perror(error_message);
    exit(exit_code);
}

key_t generate_key(int id){
    char *home = getenv("HOME");
    if (home == NULL) exit_error("No find environment variable ", -1);
    key_t key = ftok(home, id);
    if ( key == -1 ) exit_error("Error with generating key",-1);
    return key;
}