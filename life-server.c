#include <unistd.h>
#include <ncurses.h>

#include "life-server.h"
#include "output.h"
#include "board.h"

int
main(int argc, char **argv)
{
    init();

    board *b = create_board(10, 10, 4);
    clear_board(b);

    //Glider
    place_cell(b, 1, 0);
    place_cell(b, 2, 1);
    place_cell(b, 0, 2);
    place_cell(b, 1, 2);
    place_cell(b, 2, 2);

    //Still
    /*place_cell(b, 0, 0);
    place_cell(b, 0, 1);
    place_cell(b, 1, 0);
    place_cell(b, 1, 1);*/

    print_board(b, 0);

    board *nb;
    for (int i = 0; i < 1000; i++) {
        usleep(100000);
        nb = next_generation(b);

        if (!compare_generations(b, nb)) {
            printw("Simulation finished! Stable state achieved!\n");
            destroy_board(b, 0);
            break;
        }

        print_board(nb, 1);
                
        destroy_board(b, 0);
        b = nb;
    }
    destroy_board(nb, 1);

    get_ch();

    destroy();

    return 0;
}
