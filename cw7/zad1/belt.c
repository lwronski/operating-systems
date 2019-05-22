//
// Created by lwronski on 12.05.19.
//

#include "belt.h"
#include "common.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

void take_belt(int id) {
    struct sembuf buf;
    buf.sem_num = SET_LOADER;
    buf.sem_op = -1;
    buf.sem_flg = 0;
    if (semop(id, &buf, 1) == -1) {
            exit_with_error("Error with taking SET_LOADER", -1);
    }
}

void load_to_belt(struct conveyor_belt *conveyor_belt, struct Load load) {

    conveyor_belt->queue[conveyor_belt->tail] = load;
    conveyor_belt->tail = (conveyor_belt->tail + 1) % MAX_QUEUE_SIZE;
}

void unlock_loader(int id) {

    struct sembuf buf;
    buf.sem_num = IS_LOAD;
    buf.sem_op = 1;
    buf.sem_flg = 0;
    if (semop(id, &buf, 1) == -1)
        exit_with_error("Error with taking", -1);

    buf.sem_num = SET_LOADER;

    if (semop(id, &buf, 1) == -1)
        exit_with_error("Error with unlocking", -1);
}

struct Load get_load(struct conveyor_belt *conveyor_belt) {
    struct Load temp = conveyor_belt->queue[conveyor_belt->head];
    conveyor_belt->head = (conveyor_belt->head + 1) % MAX_QUEUE_SIZE;
    return temp;
}


void wait_for_load(int id, struct conveyor_belt *conveyor_belt) {
    struct sembuf buf;
    buf.sem_num = IS_LOAD;
    buf.sem_op = -1;
    buf.sem_flg =  0;
    if (semop(id, &buf, 1) == -1) {
        exit_with_error("Error with taking", -1);
    }

    if (conveyor_belt->size == 0) {
        print_time();
        printf(" Waiting for package \n");
    }
    else {
        return;
    }

    while(1) {

        buf.sem_op = 1;

        if (semop(id, &buf, 1) == -1) {
            exit_with_error("Error with taking", -1);
        }

//        usleep(100);

        buf.sem_op = -1;

        if (semop(id, &buf, 1) == -1) {
            exit_with_error("Error with taking", -1);
        }

        if (conveyor_belt->size > 0) {
            break;
        }
    }

}


void update_belt(int id) {
    struct sembuf buf;
    buf.sem_num = IS_LOAD;
    buf.sem_op = 1;
    buf.sem_flg =  0;
    if (semop(id, &buf, 1) == -1) {
        exit_with_error("Error with taking", -1);
    }
}
