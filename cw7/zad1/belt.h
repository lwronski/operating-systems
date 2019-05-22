//
// Created by lwronski on 12.05.19.
//

#ifndef CW7_BELT_H
#define CW7_BELT_H

#include <sys/types.h>
#include <unistd.h>

#define MAX_QUEUE_SIZE 80
#define SET_LOADER 0
#define IS_LOAD 1


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

void take_belt(int id);

void load_to_belt(struct conveyor_belt *conveyor_belt, struct Load load);

void unlock_loader(int id);

struct Load get_load(struct conveyor_belt *conveyor_belt);

void wait_for_load(int id, struct conveyor_belt *conveyor_belt);

void update_belt(int id);

#endif //CW7_BELT_H
