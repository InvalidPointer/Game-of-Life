#include "life-server.h"
#include "board.h"

int
main(int argc, char **argv)
{
    board *b = create_board(10, 10);
    init_board(b);

    place_cell(b, 1, 0);
    place_cell(b, 2, 1);
    place_cell(b, 0, 2);
    place_cell(b, 1, 2);
    place_cell(b, 2, 2);

    print_board(b);

    board *nb = next_generation(b);

    print_board(nb);

    destroy_board(b);
    destroy_board(nb);

    return 0;
}
