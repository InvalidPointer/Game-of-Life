#ifndef OUTPUT_H
#define OUTPUT_H

#include "board.h"

void init(void);
void destroy(void);
char get_ch(void);

void print_board(board *b, int animate);

#endif //OUTPUT_H
