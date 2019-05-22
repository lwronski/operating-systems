//
// Created by lwronski on 27.04.19.
//

#ifndef CW6_COMMON_H
#define CW6_COMMON_H

#include "chat.h"

void exit_error(char * error_message, int exit_code);
unsigned int get_priority(enum COMMAND command);

#endif //CW6_COMMON_H
