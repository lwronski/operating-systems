//
// Created by lwronski on 27.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include "chat.h"

void exit_error(char * error_message, int exit_code){
    perror(error_message);
    exit(exit_code);
}

    unsigned int get_priority(enum COMMAND command){
        switch(command){
            case STOP:
                return 10;
            case INIT:
                return 9;
            case FRIENDS:
                return 8;
            default:
                return 1;
        }
    }