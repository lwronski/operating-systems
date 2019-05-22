#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <memory.h>
#include <errno.h>
#include <sys/resource.h>
#include <sys/time.h>

void get_time(struct rusage *rusage, struct timeval *timeval) {
    gettimeofday(timeval, 0);
    getrusage(RUSAGE_SELF, rusage);
}


void print_time(struct rusage *rusage_start_time, struct rusage *rusage_end_time, struct timeval *real_start_time,
                struct timeval *real_end_time) {

    timersub(&rusage_end_time->ru_stime, &rusage_start_time->ru_stime, &rusage_end_time->ru_stime);
    timersub(&rusage_end_time->ru_utime, &rusage_start_time->ru_utime, &rusage_end_time->ru_utime);
    timersub(real_end_time, real_start_time, real_end_time);

    printf("\tReal time:   %ld.%06ld", real_end_time->tv_sec, real_start_time->tv_usec);
    printf("\tUser time:   %ld.%06ld", rusage_end_time->ru_utime.tv_sec, rusage_end_time->ru_utime.tv_usec);
    printf("\tSystem time: %ld.%06ld \n", rusage_end_time->ru_stime.tv_sec, rusage_end_time->ru_stime.tv_usec);

}

long parse_arg_to_long(char *string) {

    char *endptr;

    long value = strtol(string, &endptr, 10);

    if (errno == ERANGE || *endptr != '\0' || *string == '\0') { // TODO
        fprintf(stderr, "Error with cast argument to long \n");
        exit(1);
    }

    return value;
}

void generate_record(char *buff, long record_length) {

    long i;

    for (i = 0; i < record_length; i++) {
        buff[i] = (char) ('a' + (rand() % 25));
    }

}

void handle_exception_fread(char *buff, size_t size, size_t nmemb, FILE *file) {

    int ret_code = (int) fread(buff, size, nmemb, file);

    if (ret_code != nmemb) {
        fprintf(stderr, "Error with read file\n");
        exit(1);
    }

}

void handle_exception_fwrite(char *buff, size_t size, size_t nmemb, FILE *file) {

    int ret_code = (int) fwrite(buff, size, nmemb, file);

    if (ret_code != nmemb) {
        fprintf(stderr, "Error with write file\n");
        exit(1);
    }

}

FILE *lib_open_file(char *file_name, char *mode) {
    FILE *file = fopen(file_name, mode);

    if (file == NULL) {
        fprintf(stderr, "The file can not be opened: %s\n", file_name);
        exit(1);
    }

    return file;
}

void lib_close_file(FILE *file, char *file_name) {

    int ret_status = fclose(file);

    if (ret_status != 0) {
        fprintf(stderr, "Error with close opened file: %s\n", file_name);
    }
}

void sys_close_file(int descriptor, char *file_name) {

    int ret_status = close(descriptor);

    if (ret_status != 0) {
        fprintf(stderr, "Error with close opened file: %s\n", file_name);
    }
}

long get_size_of_file_lib( FILE *file ) {

    fseek(file, 0, SEEK_END);
    long length = ftell(file);

    rewind(file);

    return length;
}

long get_size_of_file_sys( int descriptor ) {

    long length = lseek(descriptor, 0, SEEK_END);
    return length;
}


void check_correctness_length(long records_amount, long record_length) {
    if ( record_length < 0 || records_amount < 0 ) {
        fprintf(stderr, "Record length must be positive\n");
        exit(1);
    }
}

void generate(char *file_name, long records_amount, long record_length) { // TODO add catch exception negative

    check_correctness_length(records_amount, record_length);

    long i, number_byte_written;
    char buff[record_length];
    FILE *file = lib_open_file(file_name, "w+");

    for (i = 0; i < records_amount; i++) {

        generate_record(buff, record_length);
        number_byte_written = (long) fwrite(buff, sizeof(char), (size_t) record_length, file);

        if (number_byte_written != record_length) {
            fprintf(stderr, "Error with writing to file: %s\n", file_name);
            exit(1);
        }
    }

    lib_close_file(file, file_name);

}

void lib_sort(char *file_name, long records_amount, long record_length) {

    check_correctness_length(records_amount, record_length);

    long i, j, offset;
    char *buff1 = malloc(record_length * sizeof(char));
    char *buff2 = malloc(record_length * sizeof(char));
    FILE *file = lib_open_file(file_name, "r+");

    if( get_size_of_file_lib(file) < records_amount*record_length ) {
        fprintf(stderr, "Invalid size of record_amount or record_length\n");
        lib_close_file(file, file_name);
        exit(1);
    }

    offset = record_length * sizeof(char);

    for (i = 0; i < records_amount; i++) {

        j = i;
        fseek(file, i * offset, SEEK_SET);

        if ( fread(buff1, sizeof(char), (size_t) record_length, file) != record_length ) {
            fprintf(stderr, "Error with read file\n");
            lib_close_file(file, file_name);
            exit(1);
        }


        while (j > 0) {

            fseek(file, (j - 1) * offset, SEEK_SET);
            if ( fread(buff2, sizeof(char), (size_t) record_length, file) != record_length ) {
                fprintf(stderr, "Error with read file\n");
                lib_close_file(file, file_name);
                exit(1);
            }

            if (buff2[0] > buff1[0]) {

                fseek(file, j * offset, SEEK_SET);
                fwrite(buff2, sizeof(char), (size_t) record_length, file);

                j--;

            } else {
                break;
            }
        }

        if (i != j) {
            fseek(file, (j) * offset, SEEK_SET);
            fwrite(buff1, sizeof(char), (size_t) record_length, file);
        }
    }

    free(buff1);
    free(buff2);
    lib_close_file(file, file_name);
}

void sys_sort(char *file_name, long records_amount, long record_length) {

    check_correctness_length(records_amount, record_length);

    long i, j, offset;
    char *buff1 = malloc(record_length * sizeof(char));
    char *buff2 = malloc(record_length * sizeof(char));
    int descriptor = open(file_name, O_RDWR);

    if (descriptor == -1) {
        fprintf(stderr, "The file can not be opened: %s\n", file_name);
        sys_close_file(descriptor, file_name);
        exit(1);
    }

    if( get_size_of_file_sys(descriptor) < records_amount*record_length ) {
        fprintf(stderr, "Invalid size of record_amount or record_length\n");
        sys_close_file(descriptor, file_name);
        exit(1);
    }

    offset = record_length * sizeof(char);

    for (i = 0; i < records_amount; i++) {

        j = i;

        lseek(descriptor, i * offset, SEEK_SET);
        if ( read(descriptor, buff1, (size_t) record_length) != record_length ) {
            fprintf(stderr, "Error with read file\n");
            sys_close_file(descriptor, file_name);
            exit(1);
        }

        while (j > 0) {
            lseek(descriptor, (j - 1) * offset, SEEK_SET);

            if (  read(descriptor, buff2, (size_t) record_length) != record_length ) {
                fprintf(stderr, "Error with read file\n");
                sys_close_file(descriptor, file_name);
                exit(1);
            }

            if (buff2[0] > buff1[0]) {

                lseek(descriptor, j * offset, SEEK_SET);
                write(descriptor, buff2, (size_t) record_length);

                j--;

            } else {
                break;
            }
        }

        if (i != j) {
            lseek(descriptor, (j) * offset, SEEK_SET);
            write(descriptor, buff1, (size_t) record_length);
        }
    }

    free(buff1);
    free(buff2);

    sys_close_file(descriptor, file_name);
}

void lib_copy(char *file_name_1, char *file_name_2, long records_amount, long record_length) {

    check_correctness_length(records_amount, record_length);
    if( strcmp(file_name_1, file_name_2) == 0 ){
        fprintf(stderr, "You can not copy file to the same file\n");
        exit(1);
    }

    FILE *source = lib_open_file(file_name_1, "r");
    FILE *dest = lib_open_file(file_name_2, "w+");

    char buff[record_length];

    for (long i = 0; i < records_amount; i++) {
        handle_exception_fread(buff, sizeof(char), (size_t) record_length, source);

        handle_exception_fwrite(buff, sizeof(char), (size_t) record_length, dest);
    }

    lib_close_file(source, file_name_1);
    lib_close_file(dest, file_name_2);

}

void sys_copy(char *file_name_1, char *file_name_2, long records_amount, long record_length) {

    check_correctness_length(records_amount, record_length);
    if( strcmp(file_name_1, file_name_2) == 0 ){
        fprintf(stderr, "You can not copy file to the same file\n");
        exit(1);
    }

    int source_fd = open(file_name_1, O_RDONLY);
    int dest_fd = open(file_name_2, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
    char buff[record_length];

    if (source_fd == -1) {
        fprintf(stderr, "Error with open file %s\n", file_name_1);
        sys_close_file(source_fd, file_name_1);
        sys_close_file(dest_fd, file_name_2);
        exit(1);
    }

    if (dest_fd == -1) {
        fprintf(stderr, "Error with open file %s\n", file_name_2);
        sys_close_file(source_fd, file_name_1);
        sys_close_file(dest_fd, file_name_2);
        exit(1);
    }

    for (int i = 0; i < records_amount; i++) {

        if (read(source_fd, buff, (size_t) record_length) != record_length) {
            fprintf(stderr, "Error with read file, not enough file \n");
            sys_close_file(source_fd, file_name_1);
            sys_close_file(dest_fd, file_name_2);
            exit(1);
        }


        if (write(dest_fd, buff, (size_t) record_length) != record_length) {
            fprintf(stderr, "Error with write file\n");
            sys_close_file(source_fd, file_name_1);
            sys_close_file(dest_fd, file_name_2);
            exit(1);
        }

    }


    sys_close_file(source_fd, file_name_1);
    sys_close_file(dest_fd, file_name_2);
}

int main(int argc, char **argv) {

    if ((1 == argc) || strcmp(argv[1], "-help") == 0) {

        printf("Possible operations: \n "
               "\t -generate <file_name> <number_record> <length_record> - generate file with random characters\n"
               "\t -sort <file_name> <number_record> <length_record> <allocation> - sort records in file \n"
               "\t -copy <file_source> <file_dest> <number_record> <length_record> <allocation> - copy file \n"
        );
        exit(0);
    }

    struct rusage *rusage_start_time = malloc(sizeof(struct rusage));
    struct rusage *rusage_end_time = malloc(sizeof(struct rusage));

    struct timeval *start_real_time = malloc(sizeof(struct timeval));
    struct timeval *end_real_time = malloc(sizeof(struct timeval));

    int index = 1;

    while (index < argc) {

        if (strcmp(argv[index], "-generate") == 0 && index + 3 < argc) {
            index++;
            char *file_name = argv[index++];
            long number_record = parse_arg_to_long(argv[index++]);
            long length_record = parse_arg_to_long(argv[index++]);

            get_time(rusage_start_time, start_real_time);

            generate(file_name, number_record, length_record);

            get_time(rusage_end_time, end_real_time);

            printf("Generated file %s with %ld records, %ld bytes\n", file_name, number_record, length_record);
            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);
        } else if (strcmp(argv[index], "-sort") == 0 && index + 4 < argc) {
            index++;
            char *file_name = argv[index++];
            long number_record = parse_arg_to_long(argv[index++]);
            long length_record = parse_arg_to_long(argv[index++]);
            char *allocation = argv[index++];

            get_time(rusage_start_time, start_real_time);

            if (strcmp(allocation, "sys") == 0) {
                sys_sort(file_name, number_record, length_record);
            } else {
                lib_sort(file_name, number_record, length_record);
            }

            get_time(rusage_end_time, end_real_time);

            printf("Sorted file %s with %ld records, %ld bytes with usage %s function\n", file_name, number_record,
                   length_record, allocation);
            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);

        } else if (strcmp(argv[index], "-copy") == 0 && index + 5 < argc) {
            index++;
            char *file_source = argv[index++];
            char *file_dest = argv[index++];
            long number_record = parse_arg_to_long(argv[index++]);
            long length_record = parse_arg_to_long(argv[index++]);
            char *allocation = argv[index++];

            get_time(rusage_start_time, start_real_time);

            if (strcmp(allocation, "sys") == 0) {
                sys_copy(file_source, file_dest, number_record, length_record);
            } else {
                lib_copy(file_source, file_dest, number_record, length_record);
            }

            get_time(rusage_end_time, end_real_time);

            printf("Copied file %s containing %ld records, %ld bytes to another file %s with usage %s function\n",
                   file_source, number_record, length_record, file_dest, allocation);
            print_time(rusage_start_time, rusage_end_time, start_real_time, end_real_time);
        } else {
            fprintf(stderr, "You don't specify enough arguments \n");
            exit(1);
        }


    }

    free(rusage_start_time);
    free(rusage_end_time);
    free(start_real_time);
    free(end_real_time);
    return 0;
}