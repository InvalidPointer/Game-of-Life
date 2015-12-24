#include "output.h"

#include <stdio.h>

void
print_row(char *row, int len)
{
    for (int i = 0; i < len; i++) {
        switch(row[i]) {
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
    fflush(stdout);
}
