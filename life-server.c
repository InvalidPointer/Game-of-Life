#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "commands.h"
#include "output.h"
#include "board.h"

int
main(int argc, char **argv)
{
    if (argc < 4) {
        printf("Missing some input parameters!\nUsage: life-server [height] [width] [workers count].\n");
        return 0;
    }

    int width = 0, height = 0, w_count = 0;
    height = atoi(argv[1]);
    width = atoi(argv[2]);
    w_count = atoi(argv[3]);

    if (width <= 0 || height <= 0 || w_count <= 0) {
        printf("Incorrect parameters!\n");
        return 0;
    }

    board *b = create_board(width, height, w_count);
    if (b == NULL) {
        printf("Board can not be divided! Exitting.\n");
        return 0;
    }

    key_t key = ftok("life-server", 1);
    int msgid = msgget(key, IPC_CREAT | 0666);
    cmdbuf mbuf;

    int count = 0;
    while (1) {
        if (count) {
            next_generation(b);
            count--;
        }

        msgrcv(msgid, &mbuf, CMD_SIZE, 0, 0);        
        switch(mbuf.mtype)
        {
            case S_ADD: ;
                int x = 0, y = 0;
                sscanf(mbuf.mtext, "%d %d", &x, &y);
                place_cell(b, x, y);
                break;
            case S_CLEAR:
                clear_board(b);
                break;
            case S_START: ;
                sscanf(mbuf.mtext, "%d", &count);
                break;
            case S_STOP:
                count = 0;
                break;
            case S_SNAPSHOT:
                print_board(b);
                break;
            case S_QUIT:
                destroy_board(b);
                msgctl(msgid, IPC_RMID, NULL);
                return 0;
            default:
                continue;
        }
    }

    return 0;
}
