
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
#include <sys/time.h>
#include "monitor.h"
#include "file_service.h"
#include <sys/time.h>
#include <sys/resource.h>

struct process_status *pids_status;
int number_of_lines = 0;

void list(struct process_status *pids_status, int lines);

void stop_all(struct process_status *pids_status, int number_of_lines);

void start_all(struct process_status *pids_status, int number_of_lines);

void stop_pid(int pid);

void start_pid(int pid);

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

    if (path_file == NULL || second_string == NULL) {
        fprintf(stderr, "Error with parsing data in file\n");
        exit(1);
    }

    long second = parse_arg_to_long(second_string);
    if (second <= 0) {
        fprintf(stderr, "Second must be positive\n");
        exit(1);
    }

    record_data->time = second;
    record_data->path = path_file;
}

int start_monitor_file(char *file_name, struct process_status *pids_status) {

    FILE *file = open_file(file_name, "r");

    char *line = NULL;
    size_t size = 0, length_record_data_path;
    struct file_data *record_data = malloc(sizeof(struct file_data));
    pid_t pid_process;
    int i = 0, number_of_line = 0;
    struct stat file_stat;


    while (getline(&line, &size, file) != -1) {

        get_line_info(line, record_data);
        if (lstat(record_data->path, &file_stat) == 0) {
            pid_process = fork();

            if (pid_process == 0) {
                start_monitor(record_data);
            }

            length_record_data_path = strlen(record_data->path);
            pids_status[i].observe_file = malloc(length_record_data_path * sizeof(char));
            strncpy(pids_status[i].observe_file, record_data->path, length_record_data_path);
            pids_status[i].pid = pid_process;

            free(line);
            line = NULL;
            i++;
        } else {
            fprintf(stderr, "Error with parsing arguments in %d line\n", number_of_line);
        }
        number_of_line++;
    }

    free(line);
    free(record_data);
    return i;
}

void print_time(struct rusage *rusage_start_time, struct rusage *rusage_end_time) {

    timersub(&rusage_end_time->ru_stime, &rusage_start_time->ru_stime, &rusage_end_time->ru_stime);
    timersub(&rusage_end_time->ru_utime, &rusage_start_time->ru_utime, &rusage_end_time->ru_utime);

    printf("\tUser time:   %ld.%06ld", rusage_end_time->ru_utime.tv_sec, rusage_end_time->ru_utime.tv_usec);
    printf("\tSystem time: %ld.%06ld \n", rusage_end_time->ru_stime.tv_sec, rusage_end_time->ru_stime.tv_usec);

}

void wait_for_monitor(struct process_status *pids_status, int number_of_lines) {

    pid_t pid_process;
    int exit_code, i;
    struct rusage rusage_start_time;
    struct rusage rusage_end_time;

    for (i = 0; i < number_of_lines; i++) {

        getrusage(RUSAGE_CHILDREN, &rusage_start_time);
        kill(pids_status[i].pid, SIGINT);
        pid_process = wait(&exit_code);


        int exit_code_p = WEXITSTATUS(exit_code);

        if (exit_code != 0) {
            printf("Process %d create %d copies file \n", pid_process, exit_code_p - 1);
            getrusage(RUSAGE_CHILDREN, &rusage_end_time);
            print_time(&rusage_start_time, &rusage_end_time);
        } else {
            printf("Error with running process: %d\n", pid_process);
        }


    }

}

void print_process(int signal) {
    wait_for_monitor(pids_status, number_of_lines);
    exit(0);
}

void set_signal_handler() {
    struct sigaction action;
    action.sa_handler = &print_process;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);
}

void read_command(){
    char buf[255];
    int pid = 0;

    while (1) {

        scanf("%s", buf);
        if (strcmp(buf, "end") == 0) break;
        if (strcmp(buf, "list") == 0) {
            list(pids_status, number_of_lines);
        }
        if (strcmp(buf, "stop") == 0) {
            scanf("%s", buf);

            if (strcmp(buf, "pid") == 0) {
                scanf("%d", &pid);
                stop_pid(pid);
            } else if (strcmp(buf, "all") == 0) {
                stop_all(pids_status, number_of_lines);
            } else {
                printf("Invalid argument \n");
            }
        }
        if (strcmp(buf, "start") == 0) {
            scanf("%s", buf);

            if (strcmp(buf, "pid") == 0) {
                scanf("%d", &pid);
                start_pid(pid);
            } else if (strcmp(buf, "all") == 0) {
                start_all(pids_status, number_of_lines);
            } else {
                printf("Invalid argument \n");
            }
        }
    }
}

int main(int argc, char **argv) {

    char *path_file;

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t<path_file> \n"
        );
        exit(0);
    }

    if (argc < 2) {
        exit_with_error("You don't specify all required arguments \n", 1);
    }

    path_file = argv[1];
    number_of_lines = get_number_of_line(path_file);
    pids_status = malloc(number_of_lines * sizeof(struct process_status));
    create_directory("archiwum");

    number_of_lines = start_monitor_file(path_file, pids_status);

    set_signal_handler();
    read_command();

    return 0;
}

void stop_pid(int pid) {
    if (kill(pid, SIGUSR1) == -1) {
        perror("Error with stop process");
    } else {
        printf("Process %d stopped \n", pid);
    }
}

void list(struct process_status *pids_status, int number_of_lines) {
    int i = 0;
    for (i = 0; i < number_of_lines; i++) {
        printf("Process pid: %d observe file: %s \n", pids_status[i].pid, pids_status[i].observe_file);
    }
}

void stop_all(struct process_status *pids_status, int number_of_lines) {
    int i = 0;
    for (i = 0; i < number_of_lines; i++) {
        stop_pid(pids_status[i].pid);
    }
}

void start_pid(int pid) {
    if (kill(pid, SIGUSR2) == -1) {
        perror("Error with start process");
    } else {
        printf("Process %d started \n", pid);
    }
}

void start_all(struct process_status *pids_status, int number_of_lines) {
    int i = 0;
    for (i = 0; i < number_of_lines; i++) {
        start_pid(pids_status[i].pid);
    }
}

