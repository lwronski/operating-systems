#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "blocks_h.h"
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <ctype.h>


void get_time(struct rusage *rusage, struct timeval *timeval) {
    gettimeofday(timeval, 0);
    getrusage(RUSAGE_SELF, rusage);
}


void print_time(struct rusage *rusage_start_time, struct rusage *rusage_end_time, struct timeval *real_start_time,
                struct timeval *real_end_time) {

    timersub(&rusage_end_time->ru_stime, &rusage_start_time->ru_stime, &rusage_end_time->ru_stime);
    timersub(&rusage_end_time->ru_utime, &rusage_start_time->ru_utime, &rusage_end_time->ru_utime);
    timersub(real_end_time, real_start_time, real_end_time);

    printf("\tReal time:   %ld.%06ld", real_end_time->tv_sec, real_start_time->tv_usec);
    printf("\tUser time:   %ld.%06ld", rusage_end_time->ru_utime.tv_sec, rusage_end_time->ru_utime.tv_usec);
    printf("\tSystem time: %ld.%06ld \n", rusage_end_time->ru_stime.tv_sec, rusage_end_time->ru_stime.tv_usec);

}

void free_struct_time(struct rusage *rusage_start_time, struct rusage *rusage_end_time, struct timeval *real_start_time,
                      struct timeval *real_end_time) {
    free(rusage_start_time);
    free(rusage_end_time);
    free(real_start_time);
    free(real_end_time);
}

void clean_array_and_close_program(struct arrayBlocks *blocks, char *msg) {

    printf("%s", msg);
    delete_array_blocks(blocks);
    exit(1);
}

int check_is_positive_number(char *str) {
    int count = 0;

    while (*(str + count) != '\0') {
        if(!isdigit(*(str+count))){
            return 1;
        }
        ++count;
    }

    return 0;
}

void parse_arg(int argc, char **argv) {

    struct arrayBlocks *arrayBlocks;
    int array_size;

    struct rusage *rusage_start_time = malloc(sizeof(struct rusage));
    struct rusage *rusage_end_time = malloc(sizeof(struct rusage));

    struct timeval *start_real_time = malloc(sizeof(struct timeval));
    struct timeval *end_real_time = malloc(sizeof(struct timeval));


    if (strcmp(argv[1], "create_table") == 0) {

        if (check_is_positive_number(argv[2]) == 1) {
            free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
            printf("Invalid arguments of size table");
            exit(1);
        }

        array_size = atoi(argv[2]);

        get_time(rusage_start_time, start_real_time);

        arrayBlocks = create_blocks(array_size);

        get_time(rusage_end_time, end_real_time);

        printf("Created new array_blocks \n");
        print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);


    } else {
        printf("Firstly, you have to create tables");
        free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
        exit(1);
    }

    int index = 3;

    while (index < argc) {

        if (strcmp(argv[index], "search_directory") == 0) {

            if (index + 3 >= argc) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "All arguments were not given");
            }

            char *dir_name = argv[index + 1];
            char *file_name = argv[index + 2];
            char *temp_file_name = argv[index + 3];

            get_time(rusage_start_time, start_real_time);

            set_dir_fil_name(arrayBlocks, dir_name, file_name);
            search(arrayBlocks, temp_file_name);

            get_time(rusage_end_time, end_real_time);

            printf("Search in directory: %s files: %s and save output to temp file: %s \n", dir_name, file_name,
                   temp_file_name);

            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);

            index += 4;

        } else if (strcmp(argv[index], "remove_block") == 0) {

            if (index + 1 >= argc) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "All arguments were not given");
            }

            if (check_is_positive_number(argv[index + 1]) == 1) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "Invalid arguments of block index");
            }

            int array_block_index = atoi(argv[index + 1]);

            get_time(rusage_start_time, start_real_time);

            remove_block(arrayBlocks, array_block_index);

            get_time(rusage_end_time, end_real_time);

            printf("Removed block at index: %d \n", array_block_index);
            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);

            index += 2;


        } else if (strcmp(argv[index], "save_temp_file_to_blocks") == 0) {

            if (index + 1 >= argc) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "All arguments were not given");
            }

            char *temp_file_name = argv[index + 1];

            get_time(rusage_start_time, start_real_time);

            int index_block = save_temp_file_to_array_block(arrayBlocks, temp_file_name);

            get_time(rusage_end_time, end_real_time);

            printf("Save temp file: %s to_array blocks at index: %d \n", temp_file_name, index_block);
            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);

            index += 2;

        }
        else if (strcmp(argv[index], "search_directory_n_times") == 0) {

            if (index + 4 >= argc) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "All arguments were not given");
            }

            char *dir_name = argv[index + 1];
            char *file_name = argv[index + 2];
            char *temp_file_name = argv[index + 3];

            if (check_is_positive_number(argv[index + 4]) == 1) {
                free_struct_time(rusage_start_time,rusage_end_time, start_real_time, end_real_time);
                clean_array_and_close_program(arrayBlocks, "Invalid arguments of repeat n times");
            }

            int max_loop = atoi(argv[index+4]);


            get_time(rusage_start_time, start_real_time);

            int i;
            for (i = 0; i < max_loop; i++ ) {
                set_dir_fil_name(arrayBlocks, dir_name, file_name);
                search(arrayBlocks, temp_file_name);
                remove_block(arrayBlocks, save_temp_file_to_array_block(arrayBlocks, temp_file_name));
            }
            get_time(rusage_end_time, end_real_time);

            printf("Search n times: %d in directory: %s files: %s and save output to temp file: %s \n", max_loop, dir_name, file_name,
                   temp_file_name);

            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);

            index += 4;

        }
        else {
            break;
        }

    }

    printf("No more commands\n");
    free_struct_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);
    delete_array_blocks(arrayBlocks);
    exit(0);

}

int main(int argc, char **argv) {


    if (argc < 3) {
        printf("Not enough arguments \n");
        exit(1);
    }

    parse_arg(argc, argv);

    return 0;
}