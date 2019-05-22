//
// Created by lwronski on 27.04.19.
//

#ifndef CW6_CHAT_H
#define CW6_CHAT_H

#include <mqueue.h>

#define MESSAGE_MAX_SIZE 256
#define MAX_CLIENTS 128
#define MAX_SIZE_COMMAND 128
#define SERVER_NUMBER 41
#define NUMBER_OF_COMMAND 10
#define MAXMSG 8

enum COMMAND{
    STOP = 1,
    LIST,
    FRIENDS,
    ECHO,
    ADD,
    DEL,
    _2ALL,
    _2FRIENDS,
    _2ONE,
    INIT

};

struct Message{
    long mtype;
    mqd_t client_queue_ID;
    int client_ID;
    pid_t client_pid;
    char message[MESSAGE_MAX_SIZE];
};

struct CLIENT{
    mqd_t client_queue_ID;
    int client_ID;
    pid_t client_pid;
    int friends[MAX_CLIENTS-1];
    int number_of_friends;
};

#define MSGSZ_SIZE  sizeof(struct Message) - sizeof(long)

#endif //CW6_CHAT_H
