#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

sig_atomic_t got_sigTSTP_signal = 0;
sig_atomic_t child_pid = -1;


void create_process() {
    child_pid = fork();
    if (child_pid == 0) {
        execlp("./print_date", "./print_date", NULL);
    }
}

void kill_process(pid_t pid_process) {
    kill(pid_process, SIGKILL);
    wait(NULL);
}

void stop_signal_SIGINT(int signal) {
    printf("Odebrano sygnal SIGINT\n");
    kill_process(child_pid);
    exit(signal);
}

void continue_signal_TSTP(int signal) {

    if (child_pid != -1) {
        kill_process(child_pid);
        child_pid = -1;
    } else if (child_pid == -1 && got_sigTSTP_signal == 1) {

        create_process();
    }

    if (got_sigTSTP_signal == 0) {

        printf("Oczekuje na CTRL+Z - kontynuacja albo CTRL+C - zakonczenie programu \n");

        got_sigTSTP_signal = 1;
    } else {
        got_sigTSTP_signal = 0;
    }
}


int main(int argc, char **argv) {

    struct sigaction action;
    action.sa_handler = &continue_signal_TSTP;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;

    signal(SIGINT, &stop_signal_SIGINT);
    sigaction(SIGTSTP, &action, NULL);

    create_process();

    while (1) {
        sleep(1);

    }

    return 0;
}