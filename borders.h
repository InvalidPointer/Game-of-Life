#ifndef BORDERS_H
#define BORDERS_H

typedef struct borders
{
    int width;
    int height;

    unsigned char *top;
    unsigned char *right;
    unsigned char *bot;
    unsigned char *left;

    unsigned char *lt_corner;
    unsigned char *rt_corner;
    unsigned char *rb_corner;
    unsigned char *lb_corner;

    unsigned char shared;
} borders;

borders *create_borders(int width, int height, unsigned char shared);
void destroy_borders(borders *b);

borders *dup_borders(borders *b);

#endif //BORDERS_H