//
// Created by lwronski on 22.03.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/time.h>
#include <time.h>
#include <sys/stat.h>
#include <wait.h>
#include <unistd.h>
#include <values.h>
#include <libgen.h>
#include "monitor.h"
#include "file_service.h"

int cycle = 0;
int flag = 0;

void save_data_to_file(struct file_data *record_data, time_t last_modification) {

    char new_name_file[PATH_MAX];
    char path_copy[PATH_MAX];
    char time_char[100];

    strcpy(path_copy, record_data->path);
    strftime(time_char, 100, "%Y-%m-%d_%H-%M-%S", localtime(&last_modification));
    sprintf(new_name_file, "./archiwum/%s_%s", basename(path_copy), time_char);

    FILE *file = open_file(new_name_file, "w+");
    fwrite(record_data->data, sizeof(char), record_data->size_of_data, file);
    fclose(file);

}

void copy_ram(struct file_data *record_data, time_t last_modification) {

    if (record_data->data != NULL) save_data_to_file(record_data, last_modification);

    struct stat file_stat = get_file_info(record_data->path);
    FILE *file = open_file(record_data->path, "r");

    free(record_data->data);
    record_data->data = malloc((size_t) file_stat.st_size);

    fread(record_data->data, sizeof(char), (size_t) file_stat.st_size, file);
    record_data->size_of_data = (size_t) file_stat.st_size;
    fclose(file);
}

void stop_signal_SIGINT(int signal) {
    exit(cycle);
}

void stop_working(int signal) {
    flag = 1;
    printf("Stop working \n");
}

void start_working( int signal ) {
    flag = 0;
    printf("Start working \n");
}

void set_signal() {

    struct sigaction action;
    action.sa_handler = &stop_signal_SIGINT;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGINT, &action, NULL);

    action.sa_handler = &stop_working;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGUSR1, &action, NULL);

    action.sa_handler = &start_working;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    sigaction(SIGUSR2, &action, NULL);
}

void start_monitor(struct file_data *record_data) {

    struct timeval start_real_time;
    struct timeval start_time;
    struct timeval current_real_time;
    struct timeval time_diff;
    struct stat file_stat;
    time_t interval = record_data->time;

    set_signal();

    gettimeofday(&start_real_time, 0);
    gettimeofday(&current_real_time, 0);
    timersub(&current_real_time, &start_real_time, &time_diff);
    record_data->data = NULL;
    record_data->last_modification.tv_sec = 0;


    while (1) {

        if (flag == 0) {
            gettimeofday(&start_time, 0);

            file_stat = get_file_info(record_data->path);

            if (record_data->last_modification.tv_sec < file_stat.st_mtim.tv_sec) {

                copy_ram(record_data, file_stat.st_mtim.tv_sec);

                cycle++;
                record_data->last_modification.tv_sec = file_stat.st_mtim.tv_sec;
            }

            gettimeofday(&current_real_time, 0);
            timersub(&current_real_time, &start_time, &time_diff);

            if ((interval - time_diff.tv_sec) > 0) {
                sleep((unsigned int) (interval - time_diff.tv_sec));
            }

            gettimeofday(&current_real_time, 0);
            timersub(&current_real_time, &start_real_time, &time_diff);
        } else {
            sleep(1);
        }
    }

    exit(cycle);
}