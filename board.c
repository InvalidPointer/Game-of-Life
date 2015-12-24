#include "board.h"

#include "borders.h"
#include "output.h"

#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include <stdio.h>
#include <string.h>
#include <errno.h>

inline int
get_max(int a, int b)
{
    return (a > b) ? a : b;
}

inline int
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

inline int
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

    int commid = shmget(IPC_PRIVATE, width, IPC_CREAT | 0666);
    b->comm = shmat(commid, NULL, 0);
    shmctl(commid, IPC_RMID, NULL);

    b->shmids = malloc(b->ver_bcount * sizeof(*b->shmids));
    b->msgids = malloc(b->ver_bcount * sizeof(*b->msgids));
    for (int i = 0; i < b->ver_bcount; i++) {
        b->shmids[i] = malloc(b->hor_bcount * sizeof(**b->shmids));
        b->msgids[i] = malloc(b->hor_bcount * sizeof(**b->msgids));
        for (int j = 0; j < b->hor_bcount; j++) {
            for (int k = 0; k < 4; k++) {
                b->shmids[i][j][k] = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
            }
            b->msgids[i][j] = msgget(IPC_PRIVATE, IPC_CREAT | 0666);
        }
    }

    b->semid = semget(IPC_PRIVATE, 2, IPC_CREAT | 0666);

    for (int i = 0; i < b->ver_bcount; i++) {
        for (int j = 0; j < b->hor_bcount; j++) {
            if (!fork()) {
                int block_width = size;
                int block_height = size;
                int ver_bcount = b->ver_bcount;
                int hor_bcount = b->hor_bcount;

                if (j == b->hor_bcount - 1) {
                    block_width = width - j * size;
                }
                if (i == b->ver_bcount - 1) {
                    block_height = height - i * size;
                }

                int (**shmids)[4] = b->shmids;
                int msgid = b->msgids[i][j];
                int semid = b->semid;
                char *comm = b->comm;

                borders *i_borders = create_borders(block_width, block_height, 1);
                borders *o_borders = create_borders(block_width + 2, block_height + 2, 1);

                i_borders->lt_corner = (char *)shmat(shmids[i][j][LT_NUM], NULL, 0);
                i_borders->rt_corner = (char *)shmat(shmids[i][j][RT_NUM], NULL, 0);
                i_borders->rb_corner = (char *)shmat(shmids[i][j][RB_NUM], NULL, 0) + block_width - 1;
                i_borders->lb_corner = (char *)shmat(shmids[i][j][LB_NUM], NULL, 0) + block_height - 1;

                i_borders->top = i_borders->lt_corner + 1;
                i_borders->right = i_borders->rt_corner + 1;
                i_borders->bot = i_borders->rb_corner - block_width + 2;
                i_borders->left = i_borders->lb_corner - block_height + 2;

                int t_ind = cycl_add(i, -1, ver_bcount);
                int b_ind = cycl_add(i, 1, ver_bcount);
                int l_ind = cycl_add(j, -1, hor_bcount);
                int r_ind = cycl_add(j, 1, hor_bcount);

                o_borders->top = (char *)shmat(shmids[t_ind][j][BOT_NUM], NULL, 0);
                o_borders->right = (char *)shmat(shmids[i][r_ind][LEFT_NUM], NULL, 0);
                o_borders->bot = (char *)shmat(shmids[b_ind][j][TOP_NUM], NULL, 0);
                o_borders->left = (char *)shmat(shmids[i][l_ind][RIGHT_NUM], NULL, 0);

                o_borders->lt_corner = (char *)shmat(shmids[t_ind][l_ind][RB_NUM], NULL, 0) + block_width - 1;
                o_borders->rt_corner = (char *)shmat(shmids[t_ind][r_ind][LB_NUM], NULL, 0) + block_height - 1;
                o_borders->rb_corner = (char *)shmat(shmids[b_ind][r_ind][LT_NUM], NULL, 0);
                o_borders->lb_corner = (char *)shmat(shmids[b_ind][l_ind][RT_NUM], NULL, 0);

                block *w_block = create_block(block_width, block_height, i, j, i_borders, o_borders);
                
                for (int i = 0; i < b->ver_bcount; i++) {
                    free(b->shmids[i]);
                    free(b->msgids[i]);
                }
                free(b->shmids);
                free(b->msgids);
                free(b);

                msgbuf mbuf;
                struct sembuf sbuf = {0, -1, 0};

                while (1) {
                    msgrcv(msgid, &mbuf, MSG_SIZE, 0, 0);
                    switch(mbuf.mtype)
                    {
                        case CMD_ADD: ;
                            int x = 0, y = 0;
                            sscanf(mbuf.mtext, "%d %d", &x, &y);
                            block_place_cell(w_block, x, y);
                            break;
                        case CMD_WRK: ;
                            sbuf.sem_num = 0;
                            sbuf.sem_op = -1;
                            semop(semid, &sbuf, 1);

                            block *nw_block = process_block(w_block);
                            destroy_block(w_block, 0);
                            w_block = nw_block;

                            sbuf.sem_num = 1;
                            sbuf.sem_op = 1;
                            semop(semid, &sbuf, 1);
                            break;
                        case CMD_UPDT:
                            update_borders(w_block);
                            break;
                        case CMD_PRNT: ;
                            sbuf.sem_num = 0;
                            sbuf.sem_op = -1;
                            semop(semid, &sbuf, 1);

                            int num = 0;
                            sscanf(mbuf.mtext, "%d", &num);
                            memcpy(comm + w_block->column * size, w_block->arr[num + 1] + 1, w_block->width);

                            sbuf.sem_num = 1;
                            sbuf.sem_op = 1;
                            semop(semid, &sbuf, 1);

                            break;
                        case CMD_CLR:
                            clear_block(w_block);
                            break;
                        case CMD_END:
                            shmdt(comm);
                            destroy_block(w_block, 1);
                            exit(0);
                            break;
                        default:
                            continue;
                    }
                }
            }
        }
    }

    return b;
}

void
destroy_board(board *b)
{   
    b->msg_buf.mtype = CMD_END;

    shmdt(b->comm);
    for (int i = 0; i < b->ver_bcount; i++) {
        for (int j = 0; j < b->hor_bcount; j++) {
            msgsnd(b->msgids[i][j], &b->msg_buf, MSG_SIZE, 0);
            wait(NULL);

            for (int k = 0; k < 4; k++) {
                shmctl(b->shmids[i][j][k], IPC_RMID, NULL);
            }
            msgctl(b->msgids[i][j], IPC_RMID, NULL);
        }
        free(b->shmids[i]);
        free(b->msgids[i]);
    }

    free(b->shmids);
    free(b->msgids);
    free(b);
}

void
clear_board(board *b)
{
    b->msg_buf.mtype = CMD_CLR;
    for (int i = 0; i < b->ver_bcount; i++) {
        for (int j = 0; j < b->hor_bcount; j++) {
            msgsnd(b->msgids[i][j], &b->msg_buf, MSG_SIZE, 0);
        }
    }
}

void
place_cell(board *b, int x, int y)
{
    if (x >= 0 && x < b->width && y >= 0 && y < b->height)
    {
        int size = b->block_size;
        int row = y / size;
        int column = x / size;

        b->msg_buf.mtype = CMD_ADD;
        sprintf(b->msg_buf.mtext, "%d %d", x - size * column, y - size * row);
        msgsnd(b->msgids[row][column], &b->msg_buf, MSG_SIZE, 0);
    }
}

void
next_generation(board *b)
{
    struct sembuf sbuf = {0, b->hor_bcount * b->ver_bcount, 0};
    semop(b->semid, &sbuf, 1);

    b->msg_buf.mtype = CMD_WRK;
    for (int i = 0; i < b->ver_bcount; i++) {
        for (int j = 0; j < b->hor_bcount; j++) {
            msgsnd(b->msgids[i][j], &b->msg_buf, MSG_SIZE, 0);
        }
    }

    sbuf.sem_num = 1;
    sbuf.sem_op = -sbuf.sem_op;
    semop(b->semid, &sbuf, 1);

    b->msg_buf.mtype = CMD_UPDT;
    for (int i = 0; i < b->ver_bcount; i++) {
        for (int j = 0; j < b->hor_bcount; j++) {
            msgsnd(b->msgids[i][j], &b->msg_buf, MSG_SIZE, 0);
        }
    }
}

void
print_board(board *b)
{
    b->msg_buf.mtype = CMD_PRNT;

    int size = b->block_size;
    int height = size;
    for (int i = 0; i < b->ver_bcount; i++) {
        if (i == b->ver_bcount - 1) {
            height = b->height - i * size;
        }
        for (int num = 0; num < height; num++) {
            struct sembuf sbuf = {0, b->hor_bcount, 0};
            semop(b->semid, &sbuf, 1);
            for (int j = 0; j < b->hor_bcount; j++) {
                sprintf(b->msg_buf.mtext, "%d", num);
                msgsnd(b->msgids[i][j], &b->msg_buf, MSG_SIZE, 0);
            }
            sbuf.sem_num = 1;
            sbuf.sem_op = -sbuf.sem_op;
            semop(b->semid, &sbuf, 1);

            print_row(b->comm, b->width);
        }
    }
    printf("\n");
    fflush(stdout);
}