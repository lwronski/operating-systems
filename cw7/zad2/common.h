//
// Created by lwronski on 12.05.19.
//

#ifndef CW7_COMMON_H
#define CW7_COMMON_H

key_t get_trucker_key();

void exit_with_error(char *error_message, int exit_code);

int parse_arg_to_int(char *string);

void print_time();

#endif //CW7_COMMON_H
