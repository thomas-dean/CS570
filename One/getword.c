#include <assert.h>
#include <stdbool.h>

#include "getword.h"

/*
 * Removes white space characters from stdin. Where white space characters
 * are defined as:
 * '\t' (tab)
 * ' '  (space)
 *
 * Returns the last non-white space character found.
 */
static int rmwhitespc(void);

int getword(char *w)
{
    int c;
    int i = 0;

    c = rmwhitespc();
    if (c == '\n') {
        *w = '\0';
        return 0;
    }
    if (c == EOF) {
        *w = '\0';
        return -1;
    }

    /* Normal character; start of the word */
    *w++ = (char) c;
    i++;

    while (true) {
        c = getchar();
        switch (c) {
            case '\t':
            case ' ':
            case EOF:
                /* End of the word */
                *w = '\0';
                return i;
            case '\n':
                *w = '\0';
                ungetc('\n', stdin);
                return i;
            default:
                *w++ = (char) c;
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

static int rmwhitespc(void) {
    int c;

    while (true) {
        c = getchar();
        switch (c) {
            case '\t':
            case ' ':
                continue;
            default:
                return c;
        }
    }
}