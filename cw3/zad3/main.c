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
#include <sys/time.h>
#include <sys/resource.h>


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
        fprintf(stderr, "Seconds in file_monitor must be positive\n");
        exit(1);
    }

    record_data->time = second;
    record_data->path = path_file;
}

void start_monitor_file(char *file_name, time_t exec_time, Status status, rlim_t time_limit, rlim_t memory_limit) {

    FILE *file = open_file(file_name, "r+");

    char *line = NULL;
    size_t size = 0;
    struct file_data *record_data = malloc(sizeof(struct file_data));
    pid_t pid_process;

    struct rlimit cpu_lim;
    struct rlimit memory_lim;

    cpu_lim.rlim_cur = time_limit;
    cpu_lim.rlim_max = time_limit;
    memory_lim.rlim_cur = memory_limit * 1024 * 1024;
    memory_lim.rlim_max = memory_limit * 1024 * 1024;

    printf("Soft limit: %lu"  " bytes\n", memory_lim.rlim_max);
    printf("Soft limit: %lu"  " bytes\n", cpu_lim.rlim_max);


    while (getline(&line, &size, file) != -1) {
        get_line_info(line, record_data);

        pid_process = fork();

        if (pid_process == 0) {

            if (setrlimit(RLIMIT_CPU, &cpu_lim) != 0 || setrlimit(RLIMIT_AS, &memory_lim) != 0) {
                perror("Error with setting limit \n");
                exit(0);
            }

            start_monitor(record_data, exec_time, status);
        }

        free(line);
        line = NULL;
    }

    free(line);
    free(record_data);
}

void print_time(struct rusage *rusage_start_time, struct rusage *rusage_end_time) {

    timersub(&rusage_end_time->ru_stime, &rusage_start_time->ru_stime, &rusage_end_time->ru_stime);
    timersub(&rusage_end_time->ru_utime, &rusage_start_time->ru_utime, &rusage_end_time->ru_utime);

    printf("\tUser time:   %ld.%06ld", rusage_end_time->ru_utime.tv_sec, rusage_end_time->ru_utime.tv_usec);
    printf("\tSystem time: %ld.%06ld \n", rusage_end_time->ru_stime.tv_sec, rusage_end_time->ru_stime.tv_usec);

}

void wait_for_monitor() {

    pid_t pid_process;
    int exit_code;
    struct rusage rusage_start_time;
    struct rusage rusage_end_time;

    while (1) {

        getrusage(RUSAGE_CHILDREN, &rusage_start_time);
        pid_process = wait(&exit_code);

        if (pid_process < 0) break;

        if (WIFEXITED(exit_code)) {

            int exit_code_p = WEXITSTATUS(exit_code);

            if (exit_code != 0) {
                printf("Process %d create %d copies file \n", pid_process, exit_code_p - 1);
                getrusage(RUSAGE_CHILDREN, &rusage_end_time);
                print_time(&rusage_start_time, &rusage_end_time);
            }
        } else if (WIFSIGNALED(exit_code)) {
            fprintf(stderr, "Overload limit in PID: %d\n", pid_process);
        }

    }

}

int main(int argc, char **argv) {

    char *path_file;
    time_t time;
    Status stat_monitor;
    long time_limit, mem_limit;

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t <mode>\n"
               "\t\t 'ram' or 'exec' \n"
               "Possible operations: \n "
               "\t<path_file> <time> <mode> <time_limit> <memory_limit\n"
        );
        exit(0);
    }

    if (argc < 6) {
        exit_with_error("You don't specify all required arguments", 1);
    }

    path_file = argv[1];
    time = (time_t) parse_arg_to_long(argv[2]);
    time_limit = parse_arg_to_long(argv[4]);
    mem_limit = parse_arg_to_long(argv[5]);

    if (time <= 0) {
        exit_with_error("Time must be positive", 1);
    }

    if (time_limit <= 0) {
        exit_with_error("Time limit must be positive", 1);
    }

    if (mem_limit <= 0) {
        exit_with_error("Memory limit must be positive", 1);
    }

    if (strcmp(argv[3], "ram") == 0) {
        stat_monitor = RAM;
    } else if (strcmp(argv[3], "exec") == 0) {
        stat_monitor = EXEC;
    } else {
        fprintf(stderr, "Error with parsing 'condition' argument");
        exit(-1);
    }

    create_directory("archiwum");

    start_monitor_file(path_file, time, stat_monitor, (rlim_t) time_limit, (rlim_t) mem_limit);
    wait_for_monitor();


    return 0;
}
