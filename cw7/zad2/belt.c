//
// Created by lwronski on 12.05.19.
//

#include "belt.h"
#include "common.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <unistd.h>

void take_belt(sem_t *set_loader) {
    if (sem_wait(set_loader) == -1) exit_with_error("Taking semaphore", -1);
}

void load_to_belt(struct conveyor_belt *conveyor_belt, struct Load load) {

    conveyor_belt->queue[conveyor_belt->tail] = load;
    conveyor_belt->tail = (conveyor_belt->tail + 1) % MAX_QUEUE_SIZE;
}

void unlock_loader(sem_t *is_load, sem_t *set_loader) {

    if (sem_post(is_load) == -1)
        exit_with_error("Taking semaphore", -1);

    // usleep(10000);

    if (sem_post(set_loader) == -1)
        exit_with_error("Taking semaphore", -1);

    // usleep(10000);

}

struct Load get_load(struct conveyor_belt *conveyor_belt) {
    struct Load temp = conveyor_belt->queue[conveyor_belt->head];
    conveyor_belt->head = (conveyor_belt->head + 1) % MAX_QUEUE_SIZE;
    return temp;
}


void wait_for_load(struct conveyor_belt *conveyor_belt, sem_t *is_load) {

    if (sem_wait(is_load) == -1)
        exit_with_error("Taking semaphore", -1);

    if (conveyor_belt->size == 0) {
        print_time();
        printf(" Waiting for package \n");
    }
    else {
        return;
    }

    while(1) {

        if (sem_post(is_load) == -1)
            exit_with_error("Taking semaphore", -1);

        if (sem_wait(is_load) == -1)
            exit_with_error("Taking semaphore", -1);

        if (conveyor_belt->size > 0) {
           break;
        }
    }
}

void update_belt(sem_t *is_load) {

    if (sem_post(is_load) == -1)
        exit_with_error("Taking semaphore", -1);

}
