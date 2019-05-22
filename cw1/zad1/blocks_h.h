//
// Created by lwronski on 06.03.19.
//

#ifndef BLOCKS_H_H
#define BLOCKS_H_H

struct arrayBlocks {

    char **output_find;
    int sizeArrayBlock;
    char *name_dir;
    char *file_name;
};

struct arrayBlocks *create_blocks(int array_size);

void delete_array_blocks(struct arrayBlocks *blocks);

void set_dir_fil_name(struct arrayBlocks *blocks, char *name_dir, char *file_name);

void search(struct arrayBlocks *blocks, char *temp_file_name); //  TODO save output do temp_file

int save_temp_file_to_array_block(struct arrayBlocks *blocks, char *temp_file_name); // TODO change name

void remove_block(struct arrayBlocks *blocks, int index);

int get_length(const char *str);

#endif
