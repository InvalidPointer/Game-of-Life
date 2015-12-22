#include "board.h"

#include "borders.h"

#include <stdlib.h>

#include <stdio.h>

int
get_max(int a, int b)
{
    return (a > b) ? a : b;
}

int
check_side(int h, int w, int size, int k)
{
    return ((w - 1) / size + 1)*((h - 1) / size + 1) - k;
}

int
divide_board(int w, int h, int min, int max, int k)
{
	if (min > max) return 0;

    int middle = (min + max) / 2;
    int flag = check_side(h, w, middle, k);
    if (flag < 0) {
        return divide_board(h, w, min, middle - 1, k);
    } else if (flag > 0) {
        return divide_board(h, w, middle + 1, max, k);
    } else {
        return middle;
    }
}

int
cycl_add(int a, int b, int mod)
{
	return (a + b + mod) % mod;
}

board *
create_board(int width, int height, int block_count)
{
	board *b = malloc(sizeof(board));
    int size = divide_board(height, width, 1, get_max(height, width), block_count);
	if (!size) {
		free(b);
		return NULL;
	}

	b->width = width;
	b->height = height;

    b->block_size = size;

	b->hor_bcount = (width - 1) / size + 1;
	b->ver_bcount = (height - 1) / size + 1;

	b->workers = malloc(b->ver_bcount * sizeof(*b->workers));
	for (int i = 0; i < b->ver_bcount; i++) {
		b->workers[i] = malloc(b->hor_bcount * sizeof(**b->workers));

		for (int j = 0; j < b->hor_bcount; j++) {
			for (int k = 0; k < 4; k++) {
				b->workers[i][j].mem[k] = calloc(size, sizeof(*b->workers[0][0].mem[0]));
			}
        }
    }

    for (int i = 0; i < b->ver_bcount; i++) {
		for (int j = 0; j < b->hor_bcount; j++) {
			int block_width = size;
			int block_height = size;

			if (j == b->hor_bcount - 1) {
				block_width = width - j * size;
			}
			if (i == b->ver_bcount - 1) {
				block_height = height - i * size;
			}

			borders *i_borders = create_borders(block_width, block_height, 1);
			borders *o_borders = create_borders(block_width + 2, block_height + 2, 1);

			i_borders->top = b->workers[i][j].mem[TOP_NUM] + 1;
			i_borders->right = b->workers[i][j].mem[RIGHT_NUM] + 1;
			i_borders->bot = b->workers[i][j].mem[BOT_NUM] + 1;
			i_borders->left = b->workers[i][j].mem[LEFT_NUM] + 1;

			i_borders->lt_corner = b->workers[i][j].mem[LT_NUM];
			i_borders->rt_corner = b->workers[i][j].mem[RT_NUM];
			i_borders->rb_corner = b->workers[i][j].mem[RB_NUM] + block_width - 1;
			i_borders->lb_corner = b->workers[i][j].mem[LB_NUM] + block_height - 1;

			o_borders->top = b->workers[cycl_add(i, -1, b->ver_bcount)][j].mem[BOT_NUM];
			o_borders->right = b->workers[i][cycl_add(j, 1, b->hor_bcount)].mem[LEFT_NUM];
			o_borders->bot = b->workers[cycl_add(i, 1, b->ver_bcount)][j].mem[TOP_NUM];
			o_borders->left = b->workers[i][cycl_add(j, -1, b->hor_bcount)].mem[RIGHT_NUM];

			o_borders->lt_corner = b->workers[cycl_add(i, -1, b->ver_bcount)][cycl_add(j, -1, b->hor_bcount)].mem[RB_NUM] + block_width - 1;
			o_borders->rt_corner = b->workers[cycl_add(i, -1, b->ver_bcount)][cycl_add(j, 1, b->hor_bcount)].mem[LB_NUM] + block_height - 1;
			o_borders->rb_corner = b->workers[cycl_add(i, 1, b->ver_bcount)][cycl_add(j, 1, b->hor_bcount)].mem[LT_NUM];
			o_borders->lb_corner = b->workers[cycl_add(i, 1, b->ver_bcount)][cycl_add(j, -1, b->hor_bcount)].mem[RT_NUM];

			b->workers[i][j].bk = create_block(block_width, block_height, i_borders, o_borders);
		}
	}

	return b;
}

void
destroy_board(board *b, int finally)
{
	for (int i = 0; i < b->ver_bcount; i++) {
		for (int j = 0; j < b->hor_bcount; j++) {
			destroy_block(b->workers[i][j].bk, finally);
			if (finally) {
				for (int k = 0; k < 4; k++) {
					free(b->workers[i][j].mem[k]);
				}
			}
		}
		free(b->workers[i]);
	}
	free(b->workers);
    free(b);
}

void
clear_board(board *b)
{
    for (int i = 0; i < b->ver_bcount; i++) {
		for (int j = 0; j < b->hor_bcount; j++) {
            clear_block(b->workers[i][j].bk);
        }
    }
}

board *
dup_board(board *b)
{
	board *nb = malloc(sizeof(board));

	nb->width = b->width;
    nb->height = b->height;

    nb->block_size = b->block_size;
    nb->hor_bcount = b->hor_bcount;
    nb->ver_bcount = b->ver_bcount;

    nb->workers = malloc(nb->ver_bcount * sizeof(*nb->workers));
    for (int i = 0; i < nb->ver_bcount; i++) {
		nb->workers[i] = malloc(nb->hor_bcount * sizeof(**nb->workers));

		for (int j = 0; j < nb->hor_bcount; j++) {
			for (int k = 0; k < 4; k++) {
				nb->workers[i][j].mem[k] = b->workers[i][j].mem[k];
			}
		}
	}

	return nb;
}

void
place_cell(board *b, int x, int y)
{
	if (x >= 0 && x < b->width && y >= 0 && y < b->height)
	{
        int size = b->block_size;
		int row = y / size;
		int column = x / size;

		block_place_cell(b->workers[row][column].bk, x - size * column, y - size * row);
	}
}

board *
next_generation(board *b)
{
	board *nb = dup_board(b);

	for (int i = 0; i < nb->ver_bcount; i++) {
		for (int j = 0; j < nb->hor_bcount; j++) {
			nb->workers[i][j].bk = process_block(b->workers[i][j].bk);
		}
	}

	for (int i = 0; i < nb->ver_bcount; i++) {
		for (int j = 0; j < nb->hor_bcount; j++) {
			update_borders(nb->workers[i][j].bk);
		}
	}

	return nb;
}

int
compare_generations(board *b1, board *b2)
{
    if (b1->width != b2->width || b1->height != b2->height || b1->block_size != b2->block_size) {
        return 1;
    }

    for (int i = 0; i < b1->ver_bcount; i++) {
		for (int j = 0; j < b1->hor_bcount; j++) {
			if (compare_states(b1->workers[i][j].bk, b2->workers[i][j].bk)) {
                return 1;
            }
		}
	}

    return 0;
}
