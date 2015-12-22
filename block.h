#ifndef BLOCK_H
#define BLOCK_H

#include "borders.h"

typedef struct block
{
    int width;
    int height;

    unsigned char **arr;
    borders *inner_borders;
    borders *outer_borders;
} block;

block *create_block(int width, int height, borders *inner, borders *outer);
void destroy_block(block *b, int with_borders);

void clear_block(block *b);
void block_place_cell(block *b, int x, int y);
block *process_block(block *b);
int compare_states(block *b1, block *b2);

void load_borders(borders *from, block *to, int offset);
void save_borders(borders *to, block *from, int offset);

void update_borders(block *b);

#endif //BLOCK_H
