#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

enum
{
    S_ADD = 1,
    S_CLEAR,
    S_START,
    S_STOP,
    S_SNAPSHOT,
    S_QUIT,

    CMD_SIZE = 128
};

typedef struct cmdbuf
{
    long mtype;
    char mtext[CMD_SIZE];
} cmdbuf;


const char *CMD_STR_ADD = "add";
const char *CMD_STR_CLEAR = "clear";
const char *CMD_STR_START = "start";
const char *CMD_STR_STOP = "stop";
const char *CMD_STR_SNAPSHOT = "snapshot";
const char *CMD_STR_QUIT = "quit";