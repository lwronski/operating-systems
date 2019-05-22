//
// Created by lwronski on 05.04.19.
//

#ifndef CW5_PIPE_H
#define CW5_PIPE_H

struct command{
    char ** args;
    size_t number_of_args;
};

void execute_command(char * path);

#endif //CW5_PIPE_H
