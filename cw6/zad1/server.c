//
// Created by lwronski on 27.04.19.
//

#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <errno.h>
#include <memory.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include "chat.h"
#include "common.h"

struct CLIENT clients[MAX_CLIENTS];
key_t server_key;
int server_queue_ID;
int number_of_client;

void clean_up() {
    if (msgctl(server_queue_ID, IPC_RMID, NULL) == -1) exit_error("Deleting queue", -1);
    exit(0);
}

void call_init(struct Message message_buffer) {

    struct Message response_buffer;

    if (number_of_client + 1 > MAX_CLIENTS) {
        printf("Too many clients");
        return;
    }

    clients[number_of_client].client_queue_ID = message_buffer.client_queue_ID;
    clients[number_of_client].number_of_friends = 0;
    clients[number_of_client].client_ID = number_of_client;

    response_buffer.client_ID = number_of_client;
    response_buffer.mtype = INIT;

    if (msgsnd(clients[number_of_client].client_queue_ID, &response_buffer, MSGSZ_SIZE, 0) == -1)
        exit_error("Server sending init", -1);

    number_of_client++;
}

char *get_date() {
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    return asctime(tm);
}

void call_stop(struct Message *message_buffer) {

    if (message_buffer == NULL) {
        int i, counter = 0;
        for (i = 0; i < MAX_CLIENTS; i++) {
            if (clients[i].client_ID >= 0) {
                counter++;

                struct Message message;
                message.mtype = STOP;

                if (msgsnd(clients[i].client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
                    printf("Sending echo\n");
                }
            }
        }

        for (i = 0; i < counter; i++) {
            struct Message response_buffer;

            if (msgrcv(server_queue_ID, &response_buffer, MSGSZ_SIZE, 1, 0) == -1)
                exit_error("Error with receiving in server", -1);
            printf("Remove client id: %d server \n", response_buffer.client_ID);
        }
        exit(0);
    } else {
        printf("Remove client id: %d from server \n", message_buffer->client_ID);
        clients[message_buffer->client_ID].client_ID = -1;
    }
}

void call_echo(struct Message message_buffer) {

    char temp[MESSAGE_MAX_SIZE];
    char *date = get_date();

    sprintf(temp, "%s %s", message_buffer.message, date);

    struct Message message;
    message.mtype = ECHO;
    strcpy(message.message, temp);

    if (msgsnd(message_buffer.client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
        printf("Sending echo\n");
    }

}

void call_2all(struct Message message_buffer) {

    char temp[MESSAGE_MAX_SIZE];
    char *date = get_date();

    sprintf(temp, "clientID: %d message:%s  date:%s", message_buffer.client_ID, message_buffer.message, date);

    struct Message message;
    message.mtype = ECHO;
    strcpy(message.message, temp);

    printf("%s \n", message.message);

    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {

        if (i != message_buffer.client_ID && clients[i].client_ID != -1) {

            printf("Sending to %d \n", clients[i].client_queue_ID);

            if (msgsnd(clients[i].client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
                printf("Sending echo\n");
            }

        }
    }

}

void call_2friends(struct Message message_buffer) {

    char temp[MESSAGE_MAX_SIZE];
    char *date = get_date();

    sprintf(temp, "clientID: %d message:%s  date:%s", message_buffer.client_ID, message_buffer.message, date);

    struct Message message;
    message.mtype = ECHO;
    strcpy(message.message, temp);

    printf("%s \n", message.message);

    int client_ID = message_buffer.client_ID;

    int i;
    for (i = 0; i < clients[client_ID].number_of_friends; i++) {

        int client_queue_ID = clients[clients[client_ID].friends[i]].client_queue_ID;

        printf("Sending to %d \n", client_queue_ID);

        if (msgsnd(client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
            printf("Sending echo\n");
        }

    }

}

void call_2one(struct Message message_buffer) {

    char temp[MESSAGE_MAX_SIZE];
    char *date = get_date();

    sprintf(temp, "clientID: %d message:%s  date:%s", message_buffer.client_ID, message_buffer.message, date);

    struct Message message;
    message.mtype = ECHO;
    strcpy(message.message, temp);

    printf("%s \n", message.message);

    if (msgsnd(clients[message_buffer.client_queue_ID].client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
            printf("Sending echo\n");
    }

}

void call_list(struct Message message_buffer) {

    char temp[MESSAGE_MAX_SIZE];

    struct Message message;
    int i;

    memset(temp, 0, MESSAGE_MAX_SIZE);
    memset(message.message, 0, MESSAGE_MAX_SIZE);


    for (i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].client_ID >= 0) {
            sprintf(temp, "%sQueueID:%d  ID:%d \n", temp, clients[i].client_queue_ID, clients[i].client_ID);
        }
    }
    printf("Sending list server %s", temp);
    message.mtype = LIST;
    strcpy(message.message, temp);

    if (msgsnd(message_buffer.client_queue_ID, &message, MSGSZ_SIZE, 0) == -1) {
        perror("error");
    }

}

void call_add(struct Message message_buffer) {

    char *queue_id = strtok(message_buffer.message, ",");
    int i, client_ID = message_buffer.client_ID, is_duplicate;
    int number_of_friends = clients[client_ID].number_of_friends;

    while (queue_id != NULL && number_of_friends < MAX_CLIENTS) {
        int ID = (int) strtol(queue_id, NULL, 10);
        is_duplicate = 0;

        if (!(ID >= 0 && ID != client_ID && ID < MAX_CLIENTS)) {
            queue_id = strtok(NULL, ",");
            continue;
        }

        for (i = 0; i < number_of_friends; i++) {

            if (ID == clients[client_ID].friends[i]) {
                is_duplicate = -1;
                break;
            }

        }

        if (clients[ID].client_ID != -1 && is_duplicate == 0) {
            clients[client_ID].friends[number_of_friends] = ID;
            clients[client_ID].number_of_friends = ++number_of_friends;
            printf("Added client ID: %d  to friends %d\n", ID, client_ID);
        }

        queue_id = strtok(NULL, ",");
    }

}

void call_del(struct Message message_buffer) {

    char *queue_id = strtok(message_buffer.message, ",");
    int i, client_ID = message_buffer.client_ID;
    int number_of_friends = clients[client_ID].number_of_friends;

    while (queue_id != NULL) {

        int ID = (int) strtol(queue_id, NULL, 10);
        if (!(ID >= 0 && ID != client_ID && ID < MAX_CLIENTS)) {

            queue_id = strtok(NULL, ",");
            continue;
        }

        for (i = 0; i < number_of_friends; i++) {

            if (ID == clients[client_ID].friends[i]) {

                if (clients[ID].client_ID != -1) {

                    clients[client_ID].friends[i] = clients[client_ID].friends[number_of_friends - 1];
                    clients[client_ID].number_of_friends = --number_of_friends;
                    printf("Deleted client ID: %d  from friends %d\n", ID, client_ID);
                }

                break;
            }

        }

        queue_id = strtok(NULL, ",");
    }

}

void call_friends(struct Message message_buffer) {
    clients[message_buffer.client_ID].number_of_friends = 0;
    call_add(message_buffer);
}


void handler_SIGINT(int signal) {
    printf("Handle signal SIGINT \n");
    call_stop(NULL);
    exit(0);
}

void init_server() {
    number_of_client = 0;
    int i;
    for (i = 0; i < MAX_CLIENTS; i++) {
        clients[i].client_ID = -1;
    }

    atexit(clean_up);

    signal(SIGINT, handler_SIGINT);
    server_key = generate_key(SERVER_NUMBER);

    server_queue_ID = msgget(server_key, S_IRWXU | S_IRWXG | IPC_CREAT | IPC_EXCL);

    printf("Server queue ID: %d \n", server_queue_ID);

    if (server_queue_ID == -1) exit_error("Error with create server queue", -1);
}

int main(int argc, char **argv) {

    struct Message message_buffer;
    init_server();

    while (1) {

        if (msgrcv(server_queue_ID, &message_buffer, MSGSZ_SIZE,  -( NUMBER_OF_COMMAND), 0) == -1)
            exit_error("Error with receiving in server", -1);
        else {
            switch (message_buffer.mtype) {
                case INIT:
                    printf("Receive INIT \n");
                    call_init(message_buffer);
                    break;
                case STOP:
                    printf("Receive STOP\n");
                    call_stop(&message_buffer);
                    break;
                case ECHO:
                    printf("Receive ECHO\n");
                    call_echo(message_buffer);
                    break;
                case LIST:
                    printf("Receive LIST\n");
                    call_list(message_buffer);
                    break;
                case FRIENDS:
                    printf("Receive FRIENDS\n");
                    call_friends(message_buffer);
                    break;
                case ADD:
                    printf("Receive ADD\n");
                    call_add(message_buffer);
                    break;
                case DEL:
                    printf("Receive DEL\n");
                    call_del(message_buffer);
                    break;
                case _2ALL:
                    printf("Receive 2ALL\n");
                    call_2all(message_buffer);
                    break;
                case _2FRIENDS:
                    printf("Receive 2FRIENDS\n");
                    call_2friends(message_buffer);
                    break;
                case _2ONE:
                    printf("Receive 2ONE\n");
                    call_2one(message_buffer);
                    break;
                default:
                    continue;
            }
        }

    }

}