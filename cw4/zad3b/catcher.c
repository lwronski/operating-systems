//
// Created by lwronski on 30.03.19.
//

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <memory.h>
#include "signal_helper.h"
#include "file_service.h"

sig_atomic_t send = 0;
sig_atomic_t number_of_signal = 0;
pid_t sender_pid;
volatile sig_atomic_t flag_catcher = 0;
enum Mode mode;

void handle_SIGUSR1_catcher(int sig, siginfo_t *info, void *ucontext) {
    if (send == 0) {

        printf("Received signal catcher\n");
        number_of_signal++;
        sender_pid = info->si_pid;

        switch (mode) {
            case KILL:
                send_kill(sender_pid, SIGUSR1);
                break;
            case SIGQUEUE:
                send_sigqueue(sender_pid, SIGUSR1, 0);
                break;
            case SIGRT:
                send_kill(sender_pid, SIGUSR1);
                break;
        }
    }
}

void handle_SIGUSR2_catcher(int sig, siginfo_t *info, void *ucontext) {
    send = 1;
    sender_pid = info->si_pid;
}

void set_signal_handler() {
    struct sigaction action_1;

    sigemptyset(&action_1.sa_mask);
    action_1.sa_flags = SA_SIGINFO;
    action_1.sa_sigaction = &handle_SIGUSR1_catcher;

    struct sigaction action_2;
    sigemptyset(&action_2.sa_mask);
    action_2.sa_flags = SA_SIGINFO;
    action_2.sa_sigaction = &handle_SIGUSR2_catcher;

    sigaction(SIGUSR1, &action_1, NULL);
    sigaction(SIGRTMIN, &action_1, NULL);
    sigaction(SIGUSR2, &action_2, NULL);
    sigaction(SIGRTMAX, &action_2, NULL);
}

int main(int argc, char **argv) {

    disable_signal();

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t<mode> \n"
        );
        exit(1);
    }

    if (argc < 2) {
        exit_with_error("You don't specify all required arguments", 1);
    }

    mode = resolve_method(argv[1]);
    fprintf(stdout, "PID Catcher process: %d \n", getpid());

    while (send != 1) {

    }

    printf("Number of signal: %d  sender PID: %d \n", number_of_signal, sender_pid);
    send_signals(sender_pid, number_of_signal, mode);

    return 0;
}