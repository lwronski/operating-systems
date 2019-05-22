# define _XOPEN_SOURCE 500

#include <time.h>

#include <stdio.h>
#include <memory.h>
#include <stdlib.h>
#include <ftw.h>

#include <values.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int (*compare_function)(time_t, time_t);
time_t date_input;

int is_later(time_t a, time_t b) {
    return a > b;
}

int is_erlier(time_t a, time_t b) {
    return a < b;
}

int is_equal(time_t a, time_t b) {
    return a == b;
}

time_t parse_date(char *date) {

    struct tm * result = calloc(1, sizeof(struct tm));

    if (strptime(date, "%m-%d-%Y %H:%M:%S", result) == NULL) {
        fprintf(stderr, "Parse data failed");
        exit(1);
    }

    time_t ret_value = mktime(result);
    free(result);

    return ret_value;

}

void prepare_path(char *path, char *filename, char *concat_path) {

    strcpy(concat_path, path);
    strcat(concat_path, filename);

}

void print_file(const struct stat * fileStat, const char *ab_path) {

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

void search_files(char *path) {

    DIR *dir = opendir(path);
    struct dirent *dirent;
    struct stat * file = calloc(1, sizeof(struct stat));
    char * temp_path = calloc( PATH_MAX + 1, sizeof(char));
    strcpy(temp_path, path);

    if (dir == NULL) {
        fprintf(stderr, "Error with open directory");
        exit(1);
    }

    prepare_path(temp_path, "/", path);

    while (1) {

        errno = 0;
        memset(temp_path, 0, strlen(temp_path));
        dirent = readdir(dir);

        if (dirent != NULL) {
            if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) continue;

            prepare_path(path, dirent->d_name, temp_path);

            lstat(temp_path, file);

            if ( compare_function(file->st_mtime, date_input) == 1 )  {
                print_file(file, temp_path);
            }

            if (S_ISDIR(file->st_mode)) {
                search_files(temp_path);
            }

        } else {

            free(temp_path);
            free(file);

            if ( errno != 0) {
                fprintf(stderr, "Error with searching directory\n");
                exit(1);
            } else {
                closedir(dir);
                return;
            }
        }

    }

}

int fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {

    if ( compare_function( sb->st_mtime, date_input) == 1 ) {
        print_file(sb, fpath);
    }

    return 0;
}

int main(int argc, char **argv) {

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Arguments: \n"
               "\t <condition>\n"
               "\t\t '>' later than <date> \n"
               "\t\t '=' at the same <date> \n"
               "\t\t '=' earlier than <date> \n"
               "\t <allocation>\n"
               "\t\t 'sys' or 'lib' \n"
               "\t <date>"
               "\t\t Date must be in format mm-dd-yy hh:mm:ss"
               "Possible operations: \n "
               "\t<path_dir> <condition> <date> <allocation>\n"
        );
        exit(0);
    }

    if (argc < 3) {
        fprintf(stderr, "You don't specify all required arguments");
        exit(1);
    }

    date_input = parse_date(argv[3]);
    char condition = argv[2][0];

    char ab_path[PATH_MAX];
    if (realpath(argv[1], ab_path) == NULL) {
        fprintf(stderr, "Error with resolve absolute path\n");
        exit(1);
    };

    if (condition == '>') {
        compare_function = &is_later;
    } else if (condition == '<') {
        compare_function = &is_erlier;
    } else if (condition == '=') {
        compare_function = &is_equal;
    } else {
        fprintf(stderr, "Error with parsing 'condition' argument");
        exit(1);
    }


    if (strcmp(argv[4], "sys") == 0) {
        search_files(ab_path);
    }
    else if ( strcmp(argv[4], "nftw") == 0 ){
        if( nftw(ab_path, fn, 20, FTW_PHYS) == -1 ) {
            fprintf(stderr, "Error with searching directory\n");
            exit(1);
        }
    }
    else {
        fprintf(stderr, "Error with parsing 'condition' argument");
        exit(1);
    }

    return 0;

}
