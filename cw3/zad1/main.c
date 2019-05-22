# define _XOPEN_SOURCE 500

#include <time.h>
#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <values.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>
#include <wait.h>

void prepare_path(char *path, char *filename, char *concat_path) {

    strcpy(concat_path, path);
    strcat(concat_path, filename);

}

void print_file(const struct stat *fileStat, const char *ab_path) {

    printf("Path: %s \n\ttype: ", ab_path);
    printf((S_ISREG(fileStat->st_mode)) ? "regular file " : "");
    printf((S_ISDIR(fileStat->st_mode)) ? "directory " : "");
    printf((S_ISCHR(fileStat->st_mode)) ? "character device " : "");
    printf((S_ISBLK(fileStat->st_mode)) ? "block device " : "");
    printf((S_ISFIFO(fileStat->st_mode)) ? "FIFO " : "");
    printf((S_ISLNK(fileStat->st_mode)) ? "symbolic link " : "");
    printf((S_ISSOCK(fileStat->st_mode)) ? "socket " : "");
    printf("\n\tbytes: %li\n", fileStat->st_size);
    printf("\tlast access: %s", ctime(&fileStat->st_atime));
    printf("\tlast modification: %s", ctime(&fileStat->st_mtime));

}

int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

    if (typeflag != FTW_D) return 0;


    pid_t pid_procces = fork();

    if (pid_procces == 0) { // child process

        printf("Absolute path: %s  PID: %d \n", temp_path, getpid());
        execlp("ls", "ls", "-l", temp_path, NULL);

    } else {

       return 0;

    }


    return 0;
}


int main(int argc, char **argv) {

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t<path_dir> \n"
        );
        exit(0);
    }

    if (argc < 1) {
        fprintf(stderr, "You don't specify all required arguments");
        exit(1);
    }


    char ab_path[PATH_MAX];
    if (realpath(argv[1], ab_path) == NULL) {
        fprintf(stderr, "Error with resolve absolute path\n");
        exit(1);
    };


    if (nftw(ab_path, fn, 1000, FTW_PHYS) == -1) {
        fprintf(stderr, "Error with searching directory\n");
        exit(1);
    }

    return 0;

}
