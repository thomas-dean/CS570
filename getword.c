#include <assert.h>
#include <stdbool.h>

#include "getword.h"

/* TODO: make this a tagged union, so identifier names can be recorded */
/* TODO: add the rest of the token types */
typedef enum token {
    /* Whitespace */
    SPC_TOK,
    TAB_TOK,
    NL_TOK,
    EOF_TOK
} token_t;

static token_t gettok(void);

int getword(char *w)
{
    token_t tok;

    tok = gettok();
    switch (tok) {
        case EOF_TOK:
            *w = '\0';
            return -1;
        default:
            fprintf(stderr, "Found unimplemented token type (%d)\n", tok);
            assert(false);
    }
}

static token_t
gettok(void)
{
    /* FIXME */
    return SPC_TOK;
}
