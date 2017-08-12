#include <assert.h>
#include <stdbool.h>
#include <stdint.h>

#include "getword.h"

int getword(char *w)
{
    int c;
    uint32_t i = 0;

    /* Discard leading tabs */
    while ((c = getchar()) == '\t') {
        ;
    }
    if (c == '\n') {
        *w = '\0';
        return 0;
    }
    if (c == EOF) {
        *w = '\0';
        return -1;
    }

    /* Normal character; start of the word */
    *w++ = c;
    i++;

    while (true) {
        c = getchar();
        switch (c) {
            case '\t':
            case EOF:
                /* End of the word */
                *w = '\0';
                return i;
            case '\n':
                *w = '\0';
                ungetc('\n', stdin);
                return i;
            default:
                *w++ = c;
                i++;
                if (__builtin_expect(i == STORAGE - 1, 0)) {
                    /* We have run out of space in w buffer */
                    *w = '\0';
                    return i;
                }
        }
    }
    /* This should not be reachable. If we get here, we have a problem */
    assert(false);
}
