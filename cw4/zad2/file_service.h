//
// Created by lwronski on 24.03.19.
//

#ifndef CW3_FILE_SERVICE_H
#define CW3_FILE_SERVICE_H

void exit_with_error(const char *error, int exit_code);
FILE *open_file(char *file_name, char *mode);
struct stat get_file_info(char *path);
long parse_arg_to_long(char *string);
int get_number_of_line(char *file_name);

#endif //CW3_FILE_SERVICE_H
