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

int weight_package, number_of_cycle = -1;
int shared_memory_id = -1;
int semaphore_id = -1;
struct conveyor_belt *conveyor_belt;

void signal_INT_handler(int signo) {
    printf("Stopped Loader... \n");
    exit(0);
}

void clean() {
    shmdt(conveyor_belt);
}

void init() {

    if (atexit(clean) == -1) exit_with_error("Error with register atexit function", -1);

    key_t key_trucker = get_trucker_key();
    shared_memory_id = shmget(key_trucker, 0, 0);
    if (shared_memory_id == -1) exit_with_error("Error with creating shared memory", -1);

    conveyor_belt = shmat(shared_memory_id, NULL, 0);
    if (conveyor_belt == (void *) -1) exit_with_error("Error with getting shared memory", -1);

    semaphore_id = semget(key_trucker, 0, 0);
    if (semaphore_id == -1) exit_with_error("Error with creating semaphore", -1);
}

int wait_for_place(int flag) {

    struct sembuf buf;
    buf.sem_num = IS_LOAD;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(semaphore_id, &buf, 1) == -1) {
        exit_with_error("Error with taking SET_LOADER", -1);
    }

    if (conveyor_belt->max_weight < conveyor_belt->weight + weight_package || conveyor_belt->size == conveyor_belt->max_size) {
        if( flag == 1 ) {
            print_time();
            printf(" Waiting for access to belt %d\n", getpid());
        }
        buf.sem_op = 1;
        if (semop(semaphore_id, &buf, 1) == -1) {
            exit_with_error("Error with taking SET_LOADER", -1);
        }

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

        take_belt(semaphore_id);

        if (wait_for_place(1) == 1) {

            while (wait_for_place(0) != 0) {}

        }


        struct Load load;
        load.weight = weight_package;
        load.loader_ID = getpid();
        load_to_belt(conveyor_belt, load);

        print_time();
        printf(" Load with weight: %d loaded by: %d date: %s \n", weight_package, getpid(), ctime(&now) );

        conveyor_belt->weight += weight_package;
        conveyor_belt->size++;

        unlock_loader(semaphore_id);
        if (number_of_cycle != -1) number_of_cycle--;
    }

}
