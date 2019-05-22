//
// Created by lwronski on 05.04.19.
//

#ifndef CW5_FILE_HELPER_H
#define CW5_FILE_HELPER_H

#include <stdio.h>

void exit_with_error(const char *error, int exit_code);

FILE *open_file(char *file_name, char *mode);

#endif //CW5_FILE_HELPER_H
