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

int shared_memory_id = -1;
int semaphore_id = -1;
struct conveyor_belt *conveyor_belt;
int max_capacity_truck, max_number_load, max_weight_load;
int capacity_truck;

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

    printf("Stopped trucker");
    if (semaphore_id >= 0) {

        semctl(semaphore_id, 0, IPC_RMID);


        while (conveyor_belt->head != conveyor_belt->tail) {

            printf("%d %d \n", conveyor_belt->head, conveyor_belt->tail);
            struct Load load = get_load(conveyor_belt);

            if (load.weight + capacity_truck > max_capacity_truck) {
                printf("Track is full\n");
                new_track();
            }

            printf("Loaded by:%d  weight:%d capacity truck: %d  load on belt: %d free space to load %d\n",
                   load.loader_ID, load.weight, capacity_truck, conveyor_belt->tail  - conveyor_belt->head,
                   max_number_load - conveyor_belt->tail  + conveyor_belt->head);
            capacity_truck += load.weight;
        }

    }

    shmdt(conveyor_belt);

    if (shared_memory_id >= 0) shmctl(shared_memory_id, IPC_RMID, NULL);
}

void init() {

    if (atexit(clean) == -1) exit_with_error("Error with register atexit function", -1);

    key_t key_trucker = get_trucker_key();
    shared_memory_id = shmget(key_trucker, sizeof(struct conveyor_belt), IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
    if (shared_memory_id == -1) exit_with_error("Error with creating shared memory", -1);

    conveyor_belt = shmat(shared_memory_id, NULL, 0);
    if (conveyor_belt == (void *) -1) exit_with_error("Error with getting shared memory", -1);

    semaphore_id = semget(key_trucker, 4, IPC_CREAT | IPC_EXCL | S_IRWXU | S_IRWXG);
    if (semaphore_id == -1) exit_with_error("Error with creating semaphore", -1);

    if (semctl(semaphore_id, SET_LOADER, SETVAL, 1) == -1)
        exit_with_error("Error with setting semaphore SET_LOADER", -1);
    if (semctl(semaphore_id, IS_LOAD, SETVAL, 1) == -1)
        exit_with_error("Error with setting semaphore NUMBER_LOAD", -1);

    conveyor_belt->head = 0;
    conveyor_belt->tail = 0;
    conveyor_belt->size = 0;
    conveyor_belt->max_weight = max_weight_load;
    conveyor_belt->max_size = max_number_load;

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

    while (1) {

        wait_for_load(semaphore_id,conveyor_belt);

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


        update_belt(semaphore_id);
    }

}