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
    switch (c) {
        case '\n':
            *w = '\0';
            return 0;
        case EOF:
            *w = '\0';
            return -1;
        case '<':
        case '|':
        case '#':
        case '&':
            *w++ = (char) c;
            *w = '\0';
            return 1;
        case '>':
            /* ">" */
            c = getchar();
            if (c == '>') {
                /* ">>" */
                c = getchar();
                if (c == '&') {
                    /* ">>&" */
                    strcpy(w, ">>&");
                    return 3;
                }
                ungetc(c, stdin);
                strcpy(w, ">>");
                return 2;
            }
            if (c == '&') {
                /* ">&" */
                strcpy(w, ">&");
                return 2;
            }
            ungetc(c, stdin);
            *w++ = '>';
            *w = '\0';
            return 1;
    }

    do {
        switch (c) {
            case '\t':
            case ' ':
            case EOF:
                /* End of the word */
                *w = '\0';
                return i;
            case '<':
            case '>': /* Handles ">", ">&", ">>" and ">>&" */
            case '|':
            case '#':
            case '&':
            case '\n':
                /*
                 * We want the next call to `getword` to see this newline /
                 * meta character. This ensures that, if we have a word which
                 * is immediately followed by a newline / meta character, the
                 * first call to `getword` would return the word of interest
                 * and the next would return the empty string for the newline /
                 * meta character.
                 */
                *w = '\0';
                ungetc(c, stdin);
                return i;
            case '\\':
                /*
                 * We want to just take whatever the next character is verbatim.
                 *
                 * If we encounter a new line, ignore it and continue reading
                 * the current word. This permits commands ending in a back
                 * slash (\) to span multiple lines.
                 *
                 * If we encounter EOF, let the returned word be the single back
                 * slash (\).
                 */
                c = getchar();
                if (c == '\n') {
                    continue;
                }
                if (c == EOF) {
                    c = '\\';
                }
                /* FALLTHROUGH */
            default:
                *w++ = (char) c;
                i++;
                if (__builtin_expect(i == STORAGE - 1, 0)) {
                    /* We have run out of space in w buffer */
                    *w = '\0';
                    return i;
                }
        }
        /* Get the next character, but always loop back */
    } while ((c = getchar()), true);
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
