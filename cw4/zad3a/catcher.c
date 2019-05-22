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

void handle_SIGUSR1(int sig, siginfo_t *info, void *ucontext) {
    if (send == 0) {
        number_of_signal++;
    }
}

void handle_SIGUSR2(int sig, siginfo_t *info, void *ucontext) {
    send = 1;
    sender_pid = info->si_pid;
}

void set_signal_handler() {
    struct sigaction action_1;

    sigemptyset(&action_1.sa_mask);
    sigaddset(&action_1.sa_mask, SIGUSR2);
    sigaddset(&action_1.sa_mask, SIGRTMAX);
    action_1.sa_flags = SA_SIGINFO;
    action_1.sa_sigaction = &handle_SIGUSR1;

    struct sigaction action_2;
    sigemptyset(&action_2.sa_mask);
    action_2.sa_flags = SA_SIGINFO;
    action_2.sa_sigaction = &handle_SIGUSR2;

    sigaction(SIGUSR1, &action_1, NULL);
    sigaction(SIGRTMIN, &action_1, NULL);
    sigaction(SIGUSR2, &action_2, NULL);
    sigaction(SIGRTMAX, &action_2, NULL);
}

int main(int argc, char **argv) {

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t<mode> \n"
        );
        exit(1);
    }

    if (argc < 2) {
        exit_with_error("You don't specify all required arguments \n", 1);
    }

    enum Mode mode = resolve_method(argv[1]);


    fprintf(stdout, "PID Catcher process: %d \n", getpid());

    disable_signal();
    set_signal_handler();

    while (send != 1) {

    }

    printf("Number of signal: %d  sender PID: %d \n", number_of_signal, sender_pid);

    send_signals(sender_pid, number_of_signal, mode);

    return 0;
}