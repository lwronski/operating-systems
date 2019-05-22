//
// Created by lwronski on 12.05.19.
//

#ifndef CW7_BELT_H
#define CW7_BELT_H

#define MAX_QUEUE_SIZE 80
#define BELT_PAH "/besatp1h"
#define MAX_ELEMENT "/eslaent2"
#define MAX_WEGIGHT "/wesaght3"
#define SET_LOADER "/loasaer4"
#define IS_LOAD "/loas5"

#include <semaphore.h>


struct Load {
    int weight;
    pid_t loader_ID;
    struct timeval load_time;
};

struct conveyor_belt {
    int head;
    int tail;
    int weight;
    int max_weight;
    int size;
    int max_size;
    struct Load queue[MAX_QUEUE_SIZE];
};

void take_belt( sem_t * set_loader);

void load_to_belt(struct conveyor_belt *conveyor_belt, struct Load load);

void unlock_loader(sem_t * is_load, sem_t * set_loader);

struct Load get_load(struct conveyor_belt *conveyor_belt);

void wait_for_load(struct conveyor_belt *conveyor_belt, sem_t * is_load);

void update_belt(sem_t * is_load);

#endif //CW7_BELT_H
