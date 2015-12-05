#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "board.h"

board*
create_board(long long width, long long height)
{
    board *b = malloc(sizeof(board));

    b->width = width;
    b->height = height;

    b->arr = malloc(sizeof(unsigned char *) * height);
    for (long long i = 0; i < height; i++) {
        b->arr[i] = malloc(width * sizeof(unsigned char));
    }

    return b;

}

void
init_board(board *b)
{
    for (long long i = 0; i < b->height; i++) {
        memset(b->arr[i], 0, b->width);
    }
}

void
destroy_board(board *b)
{
    for (long long i = 0; i < b->height; i++)
        free(b->arr[i]);
    free(b->arr);

    free(b);
}

void
place_cell(board *b, long long x, long long y)
{
    b->arr[y][x] = 1;
}

char
next_state(board *b, long long x, long long y)
{
    if (x < 0 || x >= b->width || y < 0 || y >= b->height)
        return -1;

    int count = 0;
    for (long long i = x - 1; i <= x + 1; i++) {
        for (long long j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y)
                continue;

            if (i >= 0 && j >= 0 && i < b->height && j < b->width)
                if (b->arr[j][i])
                    count++;
        }
    }

    char state = !b->arr[y][x] && count == 3;                      //Empty
    state = state || (b->arr[y][x] && (count == 2 || count == 3)); //Alive

    return state;
}


board*
next_generation(board *b)
{
    board *nb = create_board(b->width, b->height);

    for (long long i = 0; i < b->height; i++) {
        for (long long j = 0; j < b->width; j++) {
            nb->arr[i][j] = next_state(b, j, i);
        }
    }

    return nb;
}

void
print_board(board *b)
{
    for (long long i = 0; i < b->height; i++) {
        for (long long j = 0; j < b->width; j++) {
            switch(b->arr[i][j]) {
                case 0:
                    printf(".");
                    break;
                case 1:
                    printf("*");
                    break;
                default:
                    printf("@");
                    break;
            }
        }
        printf("\n");
    }
    printf("\n");
}
