#include "output.h"

#include <ncurses.h>

void
init(void)
{
    initscr();
    cbreak();
}

void
destroy(void)
{
    endwin();
}

char
get_ch(void)
{
    return getch();
}
    
void
print_board(board *b, int animate)
{
    int x = 0, y = 0;
    int scr_y = 0, scr_x = 0;
    if (!animate) {
        getyx(stdscr, scr_y, scr_x);
    }
    
    for (int row = 0; row < b->ver_bcount; row++) {
		block *bk = b->workers[row][0].bk;
        int width = bk->width;
        int height = bk->height;

		for (int i = 0; i < height; i++) {
		    for (int column = 0; column < b->hor_bcount; column++) {
		        block *bk = b->workers[row][column].bk;
		        for (int j = 0; j < width; j++) {
                    x = scr_x + j + column * width;
                    y = scr_y + i + row * height;
		            switch(bk->arr[i + 1][j + 1]) {
		                case 0:
		                    mvaddch(y, x, ACS_BULLET);
		                    break;
		                case 1:
		                    mvaddch(y, x, ACS_DIAMOND);
		                    break;
		                default:
		                    mvaddch(y, x, ACS_BLOCK);
		                    break;
		            }
		        }
		    }
	    }
	    printw("\n\n");
	    refresh();
	}
}
