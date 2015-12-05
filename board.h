#ifndef BOARD_H
#define BOARD_H

typedef struct board
{
    unsigned char **arr;
    long long width, height;
} board;

board* create_board(long long width, long long height);
void init_board(board *b);
void destroy_board(board *b);

void place_cell(board *b, long long x, long long y);

char next_state(board *b, long long x, long long y);
board* next_generation(board *b);

void print_board(board* b);


#endif
