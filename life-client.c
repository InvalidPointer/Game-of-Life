#include "commands.h"

#include <stdio.h>
#include <string.h>

const char *path_to_server = "life-server";

int main(void)
{
    key_t key = ftok(path_to_server, 1);
    int msgid = msgget(key, IPC_CREAT | 0666);
    cmdbuf mbuf;

    char cmd[CMD_SIZE];
    while (1) {
        scanf("%s", cmd);

        if (!strcmp(cmd, CMD_STR_ADD)) {
            mbuf.mtype = S_ADD;         
            int x = 0, y = 0;
            scanf("%d %d", &y, &x);
            sprintf(mbuf.mtext, "%d %d", x - 1, y - 1);

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
        } else if (!strcmp(cmd, CMD_STR_CLEAR)) {
            mbuf.mtype = S_CLEAR;

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
        } else if (!strcmp(cmd, CMD_STR_START)) {
            mbuf.mtype = S_START;
            int count = 0;
            scanf("%d", &count);
            sprintf(mbuf.mtext, "%d", count);

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
        } else if (!strcmp(cmd, CMD_STR_STOP)) {
            mbuf.mtype = S_STOP;

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
        } else if (!strcmp(cmd, CMD_STR_SNAPSHOT)) {
            mbuf.mtype = S_SNAPSHOT;

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
        } else if (!strcmp(cmd, CMD_STR_QUIT)) {
            mbuf.mtype = S_QUIT;

            msgsnd(msgid, &mbuf, CMD_SIZE, 0);
            break;
        } else {
            printf("Unknown command, try again.\n");
        }
    }

    return 0;
}