# define _XOPEN_SOURCE 700

#include <time.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <values.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>
#include "monitor.h"
#include "file_service.h"



void create_directory(char *directory_name) {

    struct stat sb;

    if (!(stat(directory_name, &sb) == 0 && S_ISDIR(sb.st_mode))) {
        int ret_status = mkdir(directory_name, S_IRUSR | S_IWUSR | S_IXUSR);

        if (ret_status != 0) {
            exit_with_error("Error with create directory", 1);
        }
    }

}

void get_line_info(char *line, struct file_data *record_data) {

    char *path_file = strtok(line, " ");
    char *second_string = strtok(NULL, "\n");

    long second = parse_arg_to_long(second_string);
    if (second <= 0) {
        fprintf(stderr, "Second must be positive\n");
        exit(1);
    }

    record_data->time = second;
    record_data->path = path_file;
}

void (char *file_name, time_t exec_time, Status status) {

    FILE *file = open_file(file_name, "r");

    char *line = NULL;
    size_t size = 0;
    struct file_data *record_data = malloc(sizeof(struct file_data));
    pid_t pid_process;

    while (getline(&line, &size, file) != -1) {
        get_line_info(line, record_data);

        pid_process = fork();

        if (pid_process == 0) {
            start_monitor(record_data, exec_time, status);
        }

        free(line);
        line = NULL;
    }

    free(line);
    free(record_data);
}

void wait_for_monitor() {

    pid_t pid_process;
    int exit_code;

    while (1) {

        pid_process = wait(&exit_code);
        if (pid_process < 0) break;

        if (WIFEXITED(exit_code)) {
            printf("Process %d create %d copies file \n", pid_process, WEXITSTATUS(exit_code));
        }

    }
}

int main(int argc, char **argv) {

    char *path_file;
    time_t time;
    Status stat_monitor;

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t <mode>\n"
               "\t\t 'ram' or 'exec' \n"
               "Possible operations: \n "
               "\t<path_file> <time> <mode>\n"
        );
        exit(0);
    }

    if (argc < 4) {
        exit_with_error("You don't specify all required arguments", 1);
    }

    path_file = argv[1];
    time = (time_t) parse_arg_to_long(argv[2]);

    if (time <= 0) {
        exit_with_error("Time must be positive", 1);
    }

    if (strcmp(argv[3], "ram") == 0) {
        stat_monitor = RAM;
    } else if (strcmp(argv[3], "exec") == 0) {
        stat_monitor = EXEC;
    } else {
        exit_with_error("Error with parsing 'condition' argument", 1);
    }

    create_directory("archiwum");

    start_monitor_file(path_file, time, stat_monitor);
    wait_for_monitor();

    return 0;
}
