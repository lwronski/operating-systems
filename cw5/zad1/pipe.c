//
// Created by lwronski on 05.04.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <zconf.h>
#include <sys/wait.h>
#include "pipe.h"
#include "file_helper.h"

#define MAX_NUMBER_ARG 256
#define MAX_NUMBER_COMMANDS 256


void get_command_from_line(char *line, char **commands, int *number_of_command) {

    *number_of_command = 0;
    commands[*number_of_command] = strtok(line, "|");

    while (*number_of_command < MAX_NUMBER_COMMANDS && commands[*number_of_command] != NULL) {
        *number_of_command += 1;
        commands[*number_of_command] = strtok(NULL, "|");
    }

}

struct command *get_arg(char *command_char) {

    struct command *parse_command = malloc(sizeof(struct command));
    parse_command->args = calloc(MAX_NUMBER_ARG, sizeof(char *));

    size_t size_command = strlen(command_char);
    size_t i, index = 0;
    char buf[size_command+1];
    int flag = 0;

    for (i = 0; i < size_command; i++) {
        if (command_char[i] != ' ' && command_char[i] != '\t' && command_char[i] != '\n') {
            buf[index++] = command_char[i];
            flag = 1;
        } else {
            if (flag == 1) {
                buf[index++] = ' ';
            }
        }
    }

    buf[index] = '\0';

    index = 0;
    char *temp = NULL;
    temp = strtok(buf, " ");

    while (temp != NULL) {

        parse_command->args[index] = malloc(sizeof(char) * (strlen(temp) + 1));
        strcpy(parse_command->args[index++], temp);
        temp = strtok(NULL, " ");
    }

    parse_command->number_of_args = index;

    return parse_command;

}

void run_command(struct command **command_with_arg, int number_of_command) {

    int fd_save[2], fd_ch[2];
    pid_t pid;

    for (int i = 0; i < number_of_command; i++) {

        if (pipe(fd_ch) != 0) {
            fprintf(stderr, "Error with pipe \n");
            exit(1);
        };

        pid = fork();

        if (pid == 0) {

            close(fd_ch[0]);

            dup2(fd_save[0], STDIN_FILENO);

            if (i != number_of_command -1) dup2(fd_ch[1], STDOUT_FILENO);

            execvp(command_with_arg[i]->args[0], command_with_arg[i]->args);

        } else {

            close(fd_ch[1]);

            int status;
            wait(&status);

            if (WIFEXITED(status)) {

                if( WEXITSTATUS(status) != 0) {
                    fprintf(stderr, "Error with running command\n");
                    exit(1);
                }
            }

            fd_save[0] = fd_ch[0];
        }
    }

}

void execute_command(char *path) {

    FILE *file = open_file(path, "r");

    char *line = NULL;
    char **commands = calloc(MAX_NUMBER_COMMANDS, sizeof(char *));
    struct command **command_with_arg = calloc(MAX_NUMBER_COMMANDS, sizeof(struct command *));
    size_t size = 0;
    int number_of_command;
    int i, j;

    while (getline(&line, &size, file) != -1) {

        get_command_from_line(line, commands, &number_of_command);

        for (i = 0; i < number_of_command; i++) {
            command_with_arg[i] = get_arg(commands[i]);
        }


            run_command(command_with_arg, number_of_command);

            for (i = 0; i < number_of_command; i++) {

                for (j = 0; j < command_with_arg[i]->number_of_args; j++) {
                    free(command_with_arg[i]->args[j]);
                }
                free(command_with_arg[i]->args);
                free(command_with_arg[i]);

                }

    }


    free(line);
    free(commands);
    free(command_with_arg);
    fclose(file);

}