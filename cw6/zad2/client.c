#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include "chat.h"
#include "common.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <memory.h>
#include<string.h>
#include <unistd.h>

mqd_t server_queue_ID;
mqd_t client_queue_ID;
int client_ID;
pid_t child_pid;

FILE * input;
char * queue_name;

void clean_up() {
    printf("close client\n");
    if (mq_close(client_queue_ID) == -1) exit_error("Deleting queue", -1);
    if (mq_unlink("/server") == -1) exit_error("Deleting queue", -1);
    exit(0);
}

void send_message(long mtype, char * msg){

    struct Message message;

    message.mtype = mtype;
    strcpy(message.message, msg);
    message.client_queue_ID = client_queue_ID;
    message.client_ID = client_ID;

    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(mtype)) == -1) exit_error("Sending", -1);

}

void call_init() {

    struct Message message;
    struct Message return_buf;

    message.mtype = INIT;
    message.client_queue_ID = client_queue_ID;
    strcpy(message.message, queue_name);

    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(INIT)) == -1) exit_error("Sending init", -1);

    if (mq_receive(client_queue_ID, (char *) &return_buf, MESSAGE_MAX_SIZE, NULL) == -1) exit_error("Sending init server", -1); // TODO

    client_ID = return_buf.client_ID;
}

void call_stop() {

    printf("Call stop %d\n", child_pid);


    struct Message message;
    message.mtype = STOP;
    message.client_ID = client_ID;

    printf("%d \n", client_ID);


    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(STOP)) == -1) exit_error("Sending", -1);

    exit(0);
}

void call_echo(char * msg){

    struct Message message;

    message.mtype = ECHO;
    strcpy(message.message, msg);
    message.client_queue_ID = client_queue_ID;

    printf("%d \n",server_queue_ID );

    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(ECHO)) == -1) exit_error("Sending", -1);

}

void call_friends(char * msg){
    send_message(FRIENDS, msg);
}

void call_add(char * msg){
    send_message(ADD, msg);
}

void call_del(char * msg){
    send_message(DEL,msg);
}

void call_2all(char * msg){
    send_message(_2ALL,msg);
}

void call_2friends(char * msg){
    send_message(_2FRIENDS,msg);
}

void call_2one(char * id, char * msg){
    struct Message message;

    message.mtype = _2ONE;
    strcpy(message.message, msg);
    message.client_queue_ID = (int) strtol(id, NULL, 10);
    message.client_ID = client_ID;

    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(_2ONE)) == -1) exit_error("Sending", -1);
}


void call_list(){
    struct Message message;

    message.mtype = LIST;
    message.client_queue_ID = client_queue_ID;

    if (mq_send(server_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, get_priority(LIST)) == -1) exit_error("Sending", -1);

}

void handler_SIGINT(int signal) {
    call_stop();
    exit(0);
}

void read_command(){

    char * line = NULL;
    size_t size = 0;
    char command[MAX_SIZE_COMMAND];
    char arg[MAX_SIZE_COMMAND];
    char arg_2[MAX_SIZE_COMMAND];

    while (getline(&line, &size, input) != -1) {

        int number_argument = sscanf(line, "%s %s %s", command, arg, arg_2);
        if (strcmp(command, "stop") == 0 && number_argument == 1) {
            call_stop();
        }
        else if( strcmp(command, "echo") == 0 && number_argument == 2){
            call_echo(arg);
        }
        else if( strcmp(command, "list") == 0 && number_argument == 1){
            call_list();
        }
        else if( strcmp(command, "friends") == 0 && ( number_argument == 1 || number_argument == 2) ){
            call_friends(arg);
        }
        else if( strcmp(command, "add") == 0 && ( number_argument == 2) ){
            call_add(arg);
        }
        else if( strcmp(command, "del") == 0 && ( number_argument == 2) ){
            call_del(arg);
        }
        else if( strcmp(command, "2all") == 0 && ( number_argument == 2) ){
            call_2all(arg);
        }
        else if( strcmp(command, "2friends") == 0 && ( number_argument == 2) ){
            call_2friends(arg);
        }
        else if( strcmp(command, "2one") == 0 && ( number_argument == 3) ){
            call_2one(arg, arg_2);
        }
        else {
            printf("Invalid argument\n");
        }

    }
}

int main(int argc, char **argv) {

    if ( argc == 2 ){
        input = fopen(argv[1], "r");
    }
    else {
        input = stdin;
    }

    queue_name = malloc(32*sizeof(char));
    sprintf(queue_name, "/client%d%d", getpid(),rand()%1000);

    server_queue_ID = mq_open("/server", O_WRONLY);

    if (server_queue_ID == -1) exit_error("Error with opening server queue", -1);
    printf("Server queue ID: %d \n", server_queue_ID);

    struct mq_attr queue;
    queue.mq_maxmsg = MAXMSG;
    queue.mq_msgsize = MESSAGE_MAX_SIZE;
    client_queue_ID = mq_open(queue_name, O_RDONLY | O_CREAT | O_EXCL,  0666, &queue);

    if (client_queue_ID == -1) exit_error("Error with opening client queue", -1);

    printf("Client queue ID: %d \n", client_queue_ID);

    call_init();

    child_pid = fork();

    if ( child_pid != 0 ) {

        signal(SIGINT, handler_SIGINT);
        atexit(clean_up);
        read_command();

    }
    else {
        while(1){

            struct Message message;

            if (mq_receive(client_queue_ID, (char *) &message, MESSAGE_MAX_SIZE, NULL) == -1) {
                printf("Close queue\n");
                exit(0);
            }

            if( message.mtype != STOP) {
                printf("Receive message from server:  \n%s", message.message);
            }
            else{
                kill(child_pid, SIGINT);
                printf("Close queue\n");
                exit(0);
            }
        }
    }

    return 0;
}