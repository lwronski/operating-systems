#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>
#include <sys/time.h>
#include <time.h>

sig_atomic_t got_sigTSTP_signal = 0;

void stop_signal_SIGINT(int signal) {
    printf("Odebrano sygnal SIGINT\n");
    exit(signal);
}

void continue_signal_TSTP(int signal) {

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

    time_t t;

    while (1) {

        if (got_sigTSTP_signal == 0) {
            time(&t);
            printf("%s", ctime(&t));
        }
        sleep(1);

    }

    return 0;
}