//
// Created by lwronski on 01.04.19.
//

#ifndef CW3_SENDER_HELPER_H
#define CW3_SENDER_HELPER_H

enum Mode {
    KILL, SIGQUEUE, SIGRT
};

enum Mode resolve_method(char *arg);

void send_signals(pid_t catcher_pid, int number_of_singals, enum Mode mode);

void send_kill(pid_t process_pid, int sig);

void send_sigqueue(pid_t process_pid, int sig, int n);

void disable_signal();

#endif //CW3_SENDER_HELPER_H
