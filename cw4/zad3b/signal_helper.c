//
// Created by lwronski on 01.04.19.
//

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include "signal_helper.h"

enum Mode resolve_method(char *arg) {
    if (strcmp(arg, "kill") == 0) return KILL;
    else if (strcmp(arg, "sigqueue") == 0) return SIGQUEUE;
    else if (strcmp(arg, "sigrt") == 0) return SIGRT;
    else {
        fprintf(stderr, "Error with parsing method \n");
        exit(1);
    }
}

void send_kill(pid_t process_pid, int sig) {
    if (kill(process_pid, sig) != 0) {
        fprintf(stderr, "Error with sending signal to catcher \n");
        exit(1);
    }
}

void send_sigqueue(pid_t process_pid, int sig, int n) {
    union sigval val;
    val.sival_int = n;
    if (sigqueue(process_pid, sig, val) != 0) {
        fprintf(stderr, "Error with sending signal to catcher \n");
        exit(1);
    }
}

void send_signals(pid_t catcher_pid, int number_of_singals, enum Mode mode) {

    int i;
    for (i = 0; i < number_of_singals; i++) {
        switch (mode) {
            case KILL:
                send_kill(catcher_pid, SIGUSR1);
                break;
            case SIGQUEUE:
                send_sigqueue(catcher_pid, SIGUSR1, i);
                break;
            case SIGRT:
                send_kill(catcher_pid, SIGRTMIN);
                break;
        }
    }

    switch (mode) {
        case KILL:
            send_kill(catcher_pid, SIGUSR2);
            break;
        case SIGQUEUE:
            send_sigqueue(catcher_pid, SIGUSR2, i);
            break;
        case SIGRT:
            send_kill(catcher_pid, SIGRTMAX);
            break;
    }

}

void disable_signal() {
    sigset_t new_mask;

    sigfillset(&new_mask);
    sigdelset(&new_mask, SIGUSR1);
    sigdelset(&new_mask, SIGUSR2);
    sigdelset(&new_mask, SIGRTMIN);
    sigdelset(&new_mask, SIGRTMAX);

    sigprocmask(SIG_BLOCK, &new_mask, NULL);
}