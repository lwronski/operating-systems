//
// Created by lwronski on 22.03.19.
//

#ifndef CW3_MONITOR_H
#define CW3_MONITOR_H

 typedef enum {
    RAM, EXEC
} Status;

struct file_data {
    char *path;
    time_t time;
    char * data;
    size_t size_of_data;
    struct timespec last_modification;
};

int start_monitor( struct file_data * record_data, time_t exec_time, Status status);
void copy_ram(struct file_data * record_data, time_t last_modification);
FILE *open_file(char *file_name, char *mode); // TODO add new library
void exit_with_error(const char *error, int exit_code);

#endif //CW3_MONITOR_H
