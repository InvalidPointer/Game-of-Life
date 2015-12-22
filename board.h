#ifndef BOARD_H
#define BOARD_H

#include "block.h"

enum
{
    TOP_NUM = 0,
    RIGHT_NUM = 1,
    BOT_NUM = 2,
    LEFT_NUM = 3
};

enum
{
    LT_NUM = 0,
    RT_NUM = 1,
    RB_NUM = 2,
    LB_NUM = 3
};

typedef struct worker
{
    unsigned char *mem[4];
    block *bk;
} worker; 

typedef struct board
{
    int width;
    int height;

    int block_size;
    int hor_bcount;
    int ver_bcount;

    worker **workers;
} board;

board *create_board(int width, int height, int block_count);
void destroy_board(board *b, int finally);

void clear_board(board *b);
board *dup_board(board *b);

void place_cell(board *b, int x, int y);
board *next_generation(board *b);
int compare_generations(board *b1, board *b2);

#endif //BOARD_H
