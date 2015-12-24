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

enum
{   
    CMD_ADD = 1,
    CMD_WRK,
    CMD_UPDT,
    CMD_CLR,
    CMD_PRNT,
    CMD_END,
    MSG_DATA,

    MSG_SIZE = 1024
};

typedef struct msgbuf
{
    long mtype;
    char mtext[MSG_SIZE];
} msgbuf;

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
   
    int semid;

    int (**shmids)[4];
    msgbuf msg_buf;
    int **msgids;
    char *comm;
} board;

board *create_board(int width, int height, int block_count);
void destroy_board(board *b);

void clear_board(board *b);
void place_cell(board *b, int x, int y);
void next_generation(board *b);

void print_board(board *b);

#endif //BOARD_H
