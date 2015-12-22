#include "borders.h"

#include <stdlib.h>
#include <string.h>

borders *
create_borders(int width, int height, unsigned char shared)
{
    borders *b = malloc(sizeof(borders));
    b->width = width;
    b->height = height;
    b->shared = shared;

    if (!shared) {
        b->top = calloc((width - 2), sizeof(*b->top));
        b->right = calloc((height - 2), sizeof(*b->right));
        b->bot = calloc((width - 2), sizeof(*b->bot));
        b->left = calloc((height - 2), sizeof(*b->left));

        b->lt_corner = calloc(1, sizeof(*b->lt_corner));
        b->rt_corner = calloc(1, sizeof(*b->rt_corner));
        b->rb_corner = calloc(1, sizeof(*b->rb_corner));
        b->lb_corner = calloc(1, sizeof(*b->lb_corner));
    }

    return b;
}

void
destroy_borders(borders *b)
{
    if (!b->shared) {
        free(b->top);
        free(b->right);
        free(b->bot);
        free(b->left);

        free(b->lt_corner);
        free(b->rt_corner);
        free(b->rb_corner);
        free(b->lb_corner);
    }
    
    free(b);
}

borders *
dup_borders(borders *b)
{
    borders *nb = create_borders(b->width, b->height, b->shared);

    if (b->shared) {
        memcpy(nb, b, sizeof(nb));
    } else {
        *nb->lt_corner = *b->lt_corner; 
        *nb->rt_corner = *b->rt_corner;
        *nb->rb_corner = *b->rb_corner;
        *nb->lb_corner = *b->lb_corner;

        memcpy(nb->top, b->top, nb->width - 2);
        memcpy(nb->right, b->right, nb->height - 2);
        memcpy(nb->bot, b->bot, nb->width - 2);
        memcpy(nb->left, b->left, nb->height - 2);
    }

    return nb;
}