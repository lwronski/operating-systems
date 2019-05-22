//
// Created by lwronski on 07.03.19.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "blocks_h.h"


struct arrayBlocks *create_blocks(int array_size) {

    if (array_size <= 0) {
        fprintf(stderr, "The length of the table should be greater than 0");
        exit(-1);
    }

    struct arrayBlocks *blocks = calloc(1, sizeof(struct arrayBlocks));

    blocks->sizeArrayBlock = array_size;
    blocks->output_find = (char **) calloc(array_size, sizeof(char *));

    return blocks;
}


void delete_array_blocks(struct arrayBlocks *blocks) {

    if (blocks == NULL) {
        fprintf(stderr, "The arrayBlock can not be NULL");
        exit(-1);
    }


    int i;

    for (i = 0; i < blocks->sizeArrayBlock; i++) {
        remove_block(blocks, i);
    }

    free(blocks->output_find);
    free(blocks);
}


void remove_block(struct arrayBlocks *blocks, int index) {

    if (blocks == NULL) {
        fprintf(stderr, "The arrayBlock can not be NULL");
        exit(-1);
    }

    if (index < 0 || index >= blocks->sizeArrayBlock) {
        fprintf(stderr, "Invalid value of index");
        exit(-1);
    }

    free(blocks->output_find[index]);
    blocks->output_find[index] = NULL;
}

void set_dir_fil_name(struct arrayBlocks *blocks, char *name_dir, char *file_name) {

    if (blocks == NULL || name_dir == NULL || file_name == NULL) {
        fprintf(stderr, "Error with setting directory name or file name");
        exit(-1);
    }

    blocks->name_dir = name_dir;
    blocks->file_name = file_name;
}

void search(struct arrayBlocks *blocks, char *temp_file_name) {

    if (blocks == NULL || temp_file_name == NULL) {
        fprintf(stderr, "The arrayBlock  or temp_file_name can not be NULL");
        exit(-1);
    }


    int buf_size = get_length(blocks->file_name) + get_length(blocks->name_dir) + get_length(temp_file_name) + 33;
    char buf[buf_size];

    sprintf(buf, "find %s -name \"%s\" > %s 2>/dev/null", blocks->name_dir, blocks->file_name, temp_file_name);

    int ret = system(buf);

    if ( ret != 0 ){
        fprintf(stderr, "Error with execute find");
        exit(1);
    }

}

int save_temp_file_to_array_block(struct arrayBlocks *blocks, char *temp_file_name) {

    if (blocks == NULL || temp_file_name == NULL) {
        fprintf(stderr, "The arrayBlock or temp_file_name can not be NULL");
        exit(-1);
    }

    char *ptr;
    int index;

    for (index = 0; index < blocks->sizeArrayBlock; index++) {
        if (blocks->output_find[index] == NULL) {
            break;
        }
    }

    if (index == blocks->sizeArrayBlock) {

        fprintf(stderr, "Not enough array");
        exit(-1);
    }

    FILE *temp_file;
    temp_file = fopen(temp_file_name, "r");

    fseek(temp_file, 0, SEEK_END);
    int len = ftell(temp_file);

    rewind(temp_file);

    ptr = (char *) malloc(sizeof(char) * len + 1);
    ptr[len] = '\0';
    int ret_val = (int) fread(ptr, sizeof(char), len, temp_file);

    if ( ret_val == 0 ) {
        fprintf(stderr, "Error with read temp_file");
        exit(1);
    }

    blocks->output_find[index] = ptr;

    return index;
}

int get_length(const char *str)
{
    int count = 0;

    while (*(str + count) != '\0') {
        ++count;
    }

    return count;
}