#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <zconf.h>

int main(int argc, char **argv) {

    if ((argc == 1) || strcmp(argv[1], "-help") == 0) {
        printf("Arguments are: \n"
               "\t<path>\n");
        exit(0);
    }

    char * path = argv[1];

    if(mkfifo(argv[1], S_IRUSR	| S_IWUSR | S_IRGRP | S_IWGRP) != 0){
        printf("%s\n", strerror(errno));
        exit(errno);
    }

    int fd = open(path, O_RDONLY);

    if( fd == -1 ){
        printf("%s\n", strerror(errno));
        exit(errno);
    }

    char buf[PIPE_BUF + 1];

    while(1) {

        int number_bytes = read(fd, buf, PIPE_BUF);

        if( number_bytes > 0 ) {
            buf[number_bytes] = '\0';
            printf("%s \n", buf);
        }
        else {
            break;
        }

    }

    printf("Unable to read more\n");
    close(fd);
    unlink(path);

    return 0;
}