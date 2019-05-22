//
// Created by lwronski on 12.05.19.
//

#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <signal.h>
#include <time.h>
#include "stdio.h"
#include "stdlib.h"
#include "common.h"
#include "belt.h"
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

int weight_package, number_of_cycle = -1;
int shared_memory_id = -1;
int semaphore_id = -1;
struct conveyor_belt *conveyor_belt;

sem_t *semaphore_SET_LOADER = NULL, *semaphore_IS_LOAD;


void signal_INT_handler(int signo) {
    printf("Stopped Loader... \n");
    exit(0);
}

void clean() {
    munmap(conveyor_belt, sizeof(struct conveyor_belt));

    if (semaphore_SET_LOADER != NULL) {
        sem_close(semaphore_SET_LOADER);
    }

    if (semaphore_IS_LOAD != NULL) {
        sem_close(semaphore_IS_LOAD);
    }

}

void init() {

    if (atexit(clean) == -1) exit_with_error("Error with register atexit function", -1);

    shared_memory_id = shm_open(BELT_PAH, O_RDWR, S_IRWXU | S_IRWXG);
    if (shared_memory_id == -1) exit_with_error("Error with creating shared memory", -1);


    if (ftruncate(shared_memory_id, sizeof(struct conveyor_belt)) == -1)
        exit_with_error("Error with truncate memory", -1);

    conveyor_belt = mmap(NULL, sizeof(struct conveyor_belt), PROT_READ | PROT_WRITE, MAP_SHARED, shared_memory_id, 0);
    if (conveyor_belt == (void *) -1) exit_with_error("Error with getting shared memory", -1);

    semaphore_SET_LOADER = sem_open(SET_LOADER, O_RDWR);
    if (semaphore_SET_LOADER == SEM_FAILED) exit_with_error("Error with creating semaphore", -1);

    semaphore_IS_LOAD = sem_open(IS_LOAD, O_RDWR);
    if (semaphore_IS_LOAD == SEM_FAILED) exit_with_error("Error with creating semaphore", -1);

}

int wait_for_place(int flag) {

    if (sem_wait(semaphore_IS_LOAD) == -1) exit_with_error("Taking semaphore", -1);

    if (conveyor_belt->max_weight < conveyor_belt->weight + weight_package || conveyor_belt->size == conveyor_belt->max_size) {
        if( flag == 1 ) {
            print_time();
            printf(" Waiting for belt %d\n", getpid());
        }
        if (sem_post(semaphore_IS_LOAD) == -1)
            exit_with_error("Taking semaphore", -1);

        return 1;
    }
    return 0;
}

int main(int argc, char **argv) {

    time_t now;
    time(&now);

    if (argc < 2) {
        printf("Not enough argument, expected at last 1"
               "\t 'weight of package' [optional]'number_of_cycle");
    }

    weight_package = parse_arg_to_int(argv[1]);
    if (argc > 2) number_of_cycle = parse_arg_to_int(argv[2]);

    signal(SIGINT, signal_INT_handler);
    init();

    while (number_of_cycle == -1 || number_of_cycle > 0) {

        take_belt(semaphore_SET_LOADER);

        if (wait_for_place(1) == 1) {

            while (wait_for_place(0) != 0) {}

        }

        struct Load load;
        load.weight = weight_package;
        load.loader_ID = getpid();
        load_to_belt(conveyor_belt, load);

        print_time();
        printf(" Load with weight: %d loaded by: %d date: %s \n", weight_package, getpid(), ctime(&now));

        conveyor_belt->weight += weight_package;
        conveyor_belt->size++;

        unlock_loader(semaphore_IS_LOAD, semaphore_SET_LOADER);

        if (number_of_cycle != -1) number_of_cycle--;
    }

}
