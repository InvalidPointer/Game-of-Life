#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "block.h"

block *
create_block(int width, int height, int row, int column, borders *inner, borders *outer)
{
    block *b = malloc(sizeof(block));

    b->width = width;
    b->height = height;

    b->row = row;
    b->column = column;

    b->arr = malloc((height + 2) * sizeof(*b->arr));
    for (int i = 0; i < (height + 2); i++) {
        b->arr[i] = calloc((width + 2), sizeof(**b->arr));
    }

    if (inner != NULL) {
        b->inner_borders = inner;
    } else {
        b->inner_borders = create_borders(width, height, 0);
    }

    if (outer != NULL) {
        b->outer_borders = outer;
    } else {
        b->outer_borders = create_borders(width + 2, height + 2, 0);
    }

    return b;
}

void
destroy_block(block *b, int with_borders)
{
    for (int i = 0; i < b->height + 2; i++) {
        free(b->arr[i]);
    }
    free(b->arr);

    if (with_borders) {
        destroy_borders(b->inner_borders);
        //destroy_borders(b->outer_borders);
    }

    free(b);
}

void
clear_block(block *b)
{
    for (int i = 0; i < b->height + 2; i++) {
        memset(b->arr[i], 0, b->width + 2);
    }

    memset(b->inner_borders->top, 0, b->width - 2);
    memset(b->inner_borders->right, 0, b->height - 2);
    memset(b->inner_borders->bot, 0, b->width - 2);
    memset(b->inner_borders->left, 0, b->height - 2);

    *b->inner_borders->lt_corner = 0;
    *b->inner_borders->rt_corner = 0;
    *b->inner_borders->rb_corner = 0;
    *b->inner_borders->lb_corner = 0;
}

void
block_place_cell(block *b, int x, int y)
{   
    if (x >= 0 && x < b->width && y >= 0 && y < b->height) {
        b->arr[y + 1][x + 1] = 1;
    }

    update_borders(b);
}

char
next_state(block *b, int x, int y)
{
    if (x < 0 || x >= b->width || y < 0 || y >= b->height)
        return -1;

    x = x + 1; //Because block is wrapped with the outer borders
    y = y + 1;

    int count = 0;
    for (int i = x - 1; i <= x + 1; i++) {
        for (int j = y - 1; j <= y + 1; j++) {
            if (i == x && j == y)
                continue;

            count += b->arr[j][i];
        }
    }

    char state = !b->arr[y][x] && count == 3;                      //Empty
    state = state || (b->arr[y][x] && (count == 2 || count == 3)); //Alive

    return state;
}


block *
process_block(block *b)
{    
    block *nb = create_block(b->width, b->height, b->row, b->column, b->inner_borders, b->outer_borders);

    load_borders(nb->inner_borders, b, 1);
    load_borders(nb->outer_borders, b, 0);

    load_borders(nb->inner_borders, nb, 1);
    load_borders(nb->outer_borders, nb, 0);

    for (int i = 0; i < b->height; i++) {
        for (int j = 0; j < b->width; j++) {
            nb->arr[i + 1][j + 1] = next_state(b, j, i);
        }
    }

    return nb;
}

int
compare_states(block *b1, block *b2)
{
    if (b1->width != b2->width || b1->height != b2->height) {
        return 1;
    }

    for (int i = 0; i < b1->height; i++) {
        for (int j = 0; j < b1->width; j++) {
            if (b1->arr[i + 1][j + 1] != b2->arr[i + 1][j + 1])
                return 1;
        }
    }

    return 0;
}

void
load_borders(borders *from, block *to, int offset)
{
    if (from->height > to->height + 2 - offset * 2 || from->width > to->width + 2 - offset * 2) {
        return;
    }
    
    unsigned char **arr = to->arr;
    int width = to->width;
    int height = to->height;

    arr[offset][offset] = *from->lt_corner;
    arr[offset][width + 1 - offset] = *from->rt_corner;
    arr[height + 1 - offset][width + 1 - offset] = *from->rb_corner;
    arr[height + 1 - offset][offset] = *from->lb_corner;

    for (int i = 0; i < width - 2 * offset; i++) {
        arr[offset][i + 1 + offset] = from->top[i];
        arr[height + 1 - offset][i + 1 + offset] = from->bot[i];
    }

    for (int i = 0; i < height - 2 * offset; i++) {
        arr[i + 1 + offset][offset] = from->left[i];
        arr[i + 1 + offset][width + 1 - offset] = from->right[i];
    }
}

void
save_borders(borders *to, block *from, int offset)
{
    if (to->height < from->height + 2 - offset * 2 || to->width < from->width + 2 - offset * 2) {
        return;
    }
    
    unsigned char **arr = from->arr;
    int width = from->width;
    int height = from->height;

    *to->lt_corner = arr[offset][offset];
    *to->rt_corner = arr[offset][width + 1 - offset];
    *to->rb_corner = arr[height + 1 - offset][width + 1 - offset];
    *to->lb_corner = arr[height + 1 - offset][offset];

    if (offset) {
        to->left[-1] = *to->lt_corner;
        to->top[width - 2 * offset] = *to->rt_corner;
        to->right[height - 2 * offset] = *to->rb_corner;
        to->bot[-1] = *to->lb_corner;
    }
    
    for (int i = 0; i < width - 2 * offset; i++) {
        to->top[i] = arr[offset][i + 1 + offset];
        to->bot[i] = arr[height + 1 - offset][i + 1 + offset];
    }

    for (int i = 0; i < height - 2 * offset; i++) {
        to->left[i] = arr[i + 1 + offset][offset];
        to->right[i] = arr[i + 1 + offset][width + 1 - offset];
    }
}

void
update_borders(block *b)
{
    save_borders(b->inner_borders, b, 1);
}
