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
    char *p = w;

top:
    c = rmwhitespc();
    switch (c) {
        case ';':
        case '\n':
            *p = '\0';
            return 0;
        case EOF:
            *p = '\0';
            return -1;
        case '<':
        case '|':
        case '#':
        case '&':
            *p++ = (char) c;
            *p = '\0';
            return 1;
        case '>':
            /* ">" */
            *p++ = '>';
            c = getchar();
            if (c == '!') {
                /* ">!" */
                *p++ = '!';
                *p = '\0';
                return 2;
            }
            /* ">" and some other character... */
            ungetc(c, stdin);
            *p = '\0';
            return 1;
    }

    do {
        switch (c) {
            case '\t':
            case ' ':
            case EOF:
                /* End of the word */
                *p = '\0';
                goto end;
            case '<':
            case '>': /* Handles ">", ">&", ">>" and ">>&" */
            case '|':
            case '#':
            case '&':
            case ';':
            case '\n':
                /*
                 * We want the next call to `getword` to see this newline /
                 * meta character. This ensures that, if we have a word which
                 * is immediately followed by a newline / meta character, the
                 * first call to `getword` would return the word of interest
                 * and the next would return the empty string for the newline /
                 * meta character.
                 */
                *p = '\0';
                ungetc(c, stdin);
                goto end;
            case '\'':
                while (true) {
                    c = getchar();
                    if (c == '\'') {
                        /* We have encountered a closing quote */
                        break;
                    }
                    if (c == EOF) {
                        /* Encountered EOF when inside a quote */
                        fprintf(stderr, "Encountered EOF while parsing literal\n");
                        *p = '\0';
                        return i;
                    }
                    if (c == '\\') {
                        int c1 = getchar();
                        if (c1 == '\'') {
                            /* "\'" => "'" */
                            c = '\'';
                        } else {
                            /* "\" and some other character... */
                            ungetc(c1, stdin);
                        }
                    }
                    *p++ = (char) c;
                    i++;
                    if (EXPECT_FALSE(i == STORAGE - 1)) {
                        /* We have run out of space in w buffer */
                        *p = '\0';
                        return i;
                    }
                }
            case '\\':
                /*
                 * We want to just take whatever the next character is verbatim.
                 *
                 * If we encounter a new line, ignore it, clear white space
                 * and get the next word. This permits commands ending in a back
                 * slash (\) to span multiple lines.
                 *
                 * NOTE: We don't want to treat ';' and a new line the same in
                 * this situation. Here we want the literal ';'.
                 *
                 * If we encounter EOF, let the returned word be the single back
                 * slash (\).
                 */
                c = getchar();
                if (c == '\n') {
                    goto top;
                }
                if (c == EOF) {
                    c = '\\';
                }
                /* FALLTHROUGH */
            default:
                *p++ = (char) c;
                i++;
                if (EXPECT_FALSE(i == STORAGE - 1)) {
                    /* We have run out of space in w buffer */
                    *p = '\0';
                    /*
                     * No need to test if the input is "logout" since the input
                     * was more than 6 characters.
                     *
                     * This is based on the assumption that STORAGE != 7
                     */
                    return i;
                }
        }
        /* Get the next character, but always loop back */
    } while ((c = getchar()), true);
end:
    if (EXPECT_FALSE(strcmp(w, "logout") == 0)) {
        return -1;
    }
    return i;
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
