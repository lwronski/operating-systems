//
// Created by lwronski on 30.03.19.
//

#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <memory.h>
#include <unistd.h>
#include "file_service.h"
#include "signal_helper.h"

sig_atomic_t send = 0;
sig_atomic_t number_of_signal = 0;
sig_atomic_t number_of_sending_signal;

void handle_SIGUSR1(int sig, siginfo_t *info, void *ucontext) {
    if (send == 0) {
        number_of_signal++;
    }
}

void handle_SIGUSR2(int sig, siginfo_t *info, void *ucontext) {
    send = 1;
    number_of_sending_signal = info->si_value.sival_int;
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

    disable_signal();
    set_signal_handler();

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t <mode>\n"
               "\t\t 'kill' or 'sigqueue' 'sigrt' \n"
               "\t<catcher_pid> <number_of_signals> <mode> \n"
        );
        exit(1);
    }

    if (argc < 4) {
        exit_with_error("You don't specify all required arguments", 1);
    }

    pid_t catcher_pid = (pid_t) parse_arg_to_long(argv[1]);
    int number_of_singals = (int) parse_arg_to_long(argv[2]);
    enum Mode mode = resolve_method(argv[3]);

    send_signals(catcher_pid, number_of_singals, mode);

    while (send != 1) {

    }

    if (mode == SIGQUEUE) printf("Sending signals %d but arrive only %d\n", number_of_sending_signal, number_of_signal);

}