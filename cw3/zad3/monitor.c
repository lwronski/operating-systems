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

void copy_cp(struct file_data *record_data, time_t last_modification) {

    pid_t pid_process;

    pid_process = fork();

    if (pid_process == 0) {

        char new_name_file[PATH_MAX];
        char path_copy[PATH_MAX];
        char time_char[100];

        strcpy(path_copy, record_data->path);
        strftime(time_char, 100, "%Y-%m-%d_%H-%M-%S", localtime(&last_modification));
        sprintf(new_name_file, "./archiwum/%s_%s", basename(path_copy), time_char);

        execlp("cp", "cp", record_data->path, new_name_file, NULL);
    } else {
        int exit_code;
        wait(&exit_code);

        if (exit_code != 0) {
            fprintf(stderr, "Error with run cp\n");
            exit(0);
        }
    }
}

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

    if ( record_data->data == NULL ) {
        fprintf(stderr, "Not enough memory available\n");
        exit(0);
    }

    fread(record_data->data, sizeof(char), (size_t) file_stat.st_size, file);
    record_data->size_of_data = (size_t) file_stat.st_size;
    fclose(file);
}

int start_monitor(struct file_data *record_data, time_t exec_time, Status status) {

    struct timeval start_real_time;
    struct timeval start_time;
    struct timeval current_real_time;
    struct timeval time_diff;
    struct stat file_stat;
    time_t interval = record_data->time;
    int cycle = 1;


    gettimeofday(&start_real_time, 0);
    gettimeofday(&current_real_time, 0);
    timersub(&current_real_time, &start_real_time, &time_diff);
    record_data->data = NULL;
    record_data->last_modification.tv_sec = 0;


    while (time_diff.tv_sec < exec_time) {

        gettimeofday(&start_time, 0);

        file_stat = get_file_info(record_data->path);

        if (record_data->last_modification.tv_sec < file_stat.st_mtim.tv_sec) {

            switch (status) {
                case EXEC:
                    copy_cp(record_data, file_stat.st_mtim.tv_sec);
                    break;
                case RAM:
                    copy_ram(record_data, file_stat.st_mtim.tv_sec);
                    break;
            }

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
    }

    free(record_data->data);
    free(record_data);

    if (status == RAM) cycle--;

    exit(cycle);
}