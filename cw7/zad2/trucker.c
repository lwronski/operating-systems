//
// Created by lwronski on 12.05.19.
//


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include "common.h"
#include "belt.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/times.h>
#include <ctype.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <unistd.h>

int shared_memory_id = -1;
int semaphore_id = -1;
struct conveyor_belt *conveyor_belt;
int max_capacity_truck, max_number_load, max_weight_load;
int capacity_truck;
char * date;
sem_t *semaphore_SET_LOADER = NULL, *semaphore_IS_LOAD;

void signal_INT_handler(int signo) {
    printf("Stopped trucker... \n");
    exit(0);
}

void new_track() {
    printf("New truck arrived\n");
    capacity_truck = 0;
}

char *get_sub_time(struct timeval start_time, char * date) {
    struct timeval end_time;
    gettimeofday(&end_time, NULL);
    timersub(&end_time, &start_time, &end_time);
    sprintf(date, "%ld.%06lds", end_time.tv_sec, end_time.tv_usec);
    return date;
}

void clean() {

    system("killall -INT loader");

    printf("Stopped trucker");

    if (semaphore_SET_LOADER != NULL) {
        sem_close(semaphore_SET_LOADER);
        sem_unlink(SET_LOADER);
    }

    if ( shared_memory_id >= 0 && semaphore_IS_LOAD != NULL) {

        while (conveyor_belt->head != conveyor_belt->tail) {

            struct Load load = get_load(conveyor_belt);

            if (load.weight + capacity_truck > max_capacity_truck) {
                printf("Track is full\n");
                new_track();
            }

            capacity_truck += load.weight;

            printf("Loaded by:%d time: %s weight:%d capacity truck: %d  load on belt: %d free space to load %d\n",
                   load.loader_ID, get_sub_time(load.load_time, date), load.weight, capacity_truck, conveyor_belt->size,
                   conveyor_belt->max_size - conveyor_belt->size);

        }

        sem_close(semaphore_IS_LOAD);
        sem_unlink(IS_LOAD);
    }

    munmap(conveyor_belt, sizeof(struct conveyor_belt));

    if( shared_memory_id >= 0 ) {
        shm_unlink(BELT_PAH);
    }

    free(date);
}

void init() {

    if (atexit(clean) == -1) exit_with_error("Error with register atexit function", -1);

    shared_memory_id = shm_open(BELT_PAH, O_CREAT | O_RDWR | O_EXCL , 0666);
    if (shared_memory_id == -1) exit_with_error("Error with creating shared memory", -1);

    if (ftruncate(shared_memory_id, sizeof(struct conveyor_belt)) == -1) exit_with_error("Error with truncate memory", -1);


    conveyor_belt = mmap(NULL, sizeof(struct conveyor_belt),PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);
    if (conveyor_belt == (void *) -1) exit_with_error("Error with getting shared memory", -1);

    semaphore_SET_LOADER = sem_open(SET_LOADER, O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG , 1);
    if (semaphore_SET_LOADER == SEM_FAILED) exit_with_error("Error with creating semaphore", -1);

    semaphore_IS_LOAD = sem_open(IS_LOAD, O_CREAT | O_RDWR | O_EXCL, S_IRWXU | S_IRWXG , 1);
    if (semaphore_IS_LOAD == SEM_FAILED) exit_with_error("Error with creating semaphore", -1);

    conveyor_belt->head = 0;
    conveyor_belt->tail = 0;
    conveyor_belt->weight = 0;
    conveyor_belt->max_weight = max_weight_load;
    conveyor_belt->max_size = max_number_load;
    conveyor_belt->size = 0;

    new_track();
}

int main(int argc, char **argv) {

    if (argc < 4) {
        printf("Not enough arguments, expected 3 argument \n"
               "\t max_capacity_truck max_number_load max_weight_load");
        exit(0);
    }
    max_capacity_truck = parse_arg_to_int(argv[1]);
    max_number_load = parse_arg_to_int(argv[2]);
    max_weight_load = parse_arg_to_int(argv[3]);

    if (max_number_load > MAX_QUEUE_SIZE) exit_with_error("Invalid size of number of load", -1);

    signal(SIGINT, signal_INT_handler);
    init();
    date = malloc(sizeof(char) * 64);

    while (1) {


        wait_for_load(conveyor_belt, semaphore_IS_LOAD);

        struct Load load = get_load(conveyor_belt);

        if (load.weight + capacity_truck > max_capacity_truck) {
            printf("Track is full\n");
            new_track();
        }

        capacity_truck += load.weight;
        conveyor_belt->weight -= load.weight;
        conveyor_belt->size--;

        print_time();
        printf(" Loaded by:%d  weight:%d capacity truck: %d  load on belt: %d free space to load %d\n", load.loader_ID, load.weight, capacity_truck,  conveyor_belt->size, conveyor_belt->max_size-conveyor_belt->size);


        update_belt(semaphore_IS_LOAD);
    }

}