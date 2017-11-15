#include "parse.h"

parseerr_t parseerrno;

static cmd_t *newcmd(void);
static child_t *newchild(void);
static void childfree(child_t *child);
static char *readfilename(char *dst);
static void flushline(void);

cmd_t *parse(void)
{
    cmd_t *cmd;
    child_t *currchild;
    child_t *stdinredirchild = NULL;     /* The child which redirected stdin; used for sanity check */
    child_t *stdoutredirchild = NULL;    /* The child which redirected stdout; used for sanity check */
    char buf[STORAGE];
    char *p;
    char **currarg;
    token_t rc;

    if ((cmd = newcmd()) == NULL) {
        /* Failed to allocate struct */
        parseerrno = nomem;
        return NULL;
    }

    currchild = &cmd->fstchild;
    p = currchild->buf;
    currarg = currchild->childargv;
    while (true) {
        rc = getword(buf);
        if (rc == tok_errnomatch) {
            parseerrno = noquote;
            cmdfree(cmd);
            return NULL;
        }
        if (rc == tok_newline || rc == tok_semi) {
            break;
        }
        if (rc == tok_eof) {
            cmd->foundeof = true;
            break;
        }
        /* Handle meta characters... */
        if (rc == tok_pipe) {
            if (currchild->buf[0] == '\0') {
                parseerrno = nocmd;
                cmdfree(cmd);
                flushline();
                return NULL;
            }
            if ((currchild->next = newchild()) == NULL) {
                parseerrno = nomem;
                cmdfree(cmd);
                flushline();
                return NULL;
            }
            *currarg = NULL;
            currchild->next->prev = currchild;
            currchild = currchild->next;
            p = currchild->buf;
            currarg = currchild->childargv;
            continue;
        }
        if (rc == tok_amp) {
            cmd->runinbg = true;
            if (cmd->cmdstdin[0] == '\0') {
                /* if we have not already redirected stdin, we should read from
                 * /dev/null */
                strncpy(cmd->cmdstdin, "/dev/null", STORAGE);
                stdinredirchild = &cmd->fstchild; /* To sasiate the sanity checks at the end */
            }
            break;
        }
        if (rc == tok_gtbang) {
            cmd->clobber = true;
            if (cmd->cmdstdout[0] != '\0') {
                /* We have already redirected stdout! */
                parseerrno = dupredir;
                cmdfree(cmd);
                flushline();
                return NULL;
            }
            if (readfilename(cmd->cmdstdout) == NULL) {
                cmdfree(cmd);
                if (parseerrno != noquote) {
                    flushline();
                }
                return NULL;
            }
            stdoutredirchild = currchild;
            continue;
        }
        if (rc == tok_gt) {
            if (cmd->cmdstdout[0] != '\0') {
                /* We have already redirected stdout! */
                parseerrno = dupredir;
                cmdfree(cmd);
                flushline();
                return NULL;
            }
            if (readfilename(cmd->cmdstdout) == NULL) {
                cmdfree(cmd);
                if (parseerrno != noquote) {
                    flushline();
                }
                return NULL;
            }
            stdoutredirchild = currchild;
            continue;
        }
        if (rc == tok_lt) {
            if (cmd->cmdstdin[0] != '\0') {
                /* We have already redirected stdin! */
                parseerrno = dupredir;
                cmdfree(cmd);
                flushline();
                return NULL;
            }
            if (readfilename(cmd->cmdstdin) == NULL) {
                cmdfree(cmd);
                if (parseerrno != noquote) {
                    flushline();
                }
                return NULL;
            }
            stdinredirchild = currchild;
            continue;
        }
        /* Handle a regular word... */
        if (rc != tok_word) {
            fprintf(stderr, "Encountered unknown token type\n");
            parseerrno = interr;
            cmdfree(cmd);
            flushline();
            return NULL;
        }
        if (currarg - currchild->childargv == MAXITEM - 1) {
            /* we have run out of space in the argument vector */
            /* Running the command with too few arguments could lead to
             * unintended bahavior, so just bail out for this line */
            parseerrno = argvsize;
            cmdfree(cmd);
            flushline();
            return NULL;
        }
        strncpy(p, buf, STORAGE);
        *currarg++ = p;
        p += strlen(buf) + 1; /* Move p passed the word and the null terminator */
    }
    *currarg = NULL;
    cmd->lastchild = currchild;

    /* ============== Sanity checks ============== */
    /* We can't redirect stdin and stdout without specifying an executable */
    if ((cmd->cmdstdin[0] != '\0' || cmd->cmdstdout[0] != '\0') && cmd->fstchild.buf[0] == '\0') {
        parseerrno = nocmd;
        cmdfree(cmd);
        return NULL;
    }
    /* The last child process must have something in the buffer (non-empty) */
    /* But its OK to return an empty list of executables. So don't worry about
     * it if we only have one executable to run */
    if (currchild != &cmd->fstchild && currchild->buf[0] == '\0') {
        parseerrno = nocmd;
        cmdfree(cmd);
        return NULL;
    }

    /* The first child process must be the one to have stdin redirected */
    /* The last child process must be the one to have stdout redirected */
    if ((cmd->cmdstdin[0] != '\0' && stdinredirchild != &cmd->fstchild)
            || (cmd->cmdstdout[0] != '\0' && stdoutredirchild != currchild)) {
        parseerrno = dupredir;
        cmdfree(cmd);
        return NULL;
    }

    return cmd;
}

bool cmdeof(cmd_t *cmd)
{
    return cmd->foundeof;
}

bool isemptycmd(cmd_t *cmd)
{
    return cmd->clobber == false && cmd->cmdstdin[0] == '\0'
        && cmd->cmdstdout[0] == '\0' && cmd->fstchild.buf[0] == '\0';
}

void cmdfree(cmd_t *cmd)
{
    childfree(cmd->fstchild.next);
    free(cmd);
}

void pparseerr(void)
{
    switch (parseerrno) {
        case noerror:
            fprintf(stderr, "No parse error.\n");
            return;
        case noquote:
            fprintf(stderr, "Expected closing '.\n");
            return;
        case nofile:
            fprintf(stderr, "Expected a filename.\n");
            return;
        case badfile:
            fprintf(stderr, "Expected a filename; found a special character.\n");
            return;
        case nocmd:
            fprintf(stderr, "Expected an executable name.\n");
            return;
        case nomem:
            fprintf(stderr, "Out of memory.\n");
            return;
        case dupredir:
            fprintf(stderr, "Ambiguous redirection.\n");
            return;
        case argvsize:
            fprintf(stderr, "Too many arguments to an executable.\n");
            return;
        case interr:
            fprintf(stderr, "An internal error was encountered while parsing.\n");
            return;
        default:
            fprintf(stderr, "Unknown parse error occurred; error code (%d)\n", parseerrno);
    }
}

static void childfree(child_t *child)
{
    if (child == NULL) {
        return;
    }
    childfree(child->next);
    free(child);
}

static cmd_t *newcmd(void)
{
    cmd_t *rv;
    if ((rv = malloc(sizeof(cmd_t))) == NULL) {
        return NULL;
    }

    rv->foundeof = false;
    rv->runinbg = 0;
    rv->clobber = 0;
    rv->cmdstdin[0] = '\0';
    rv->cmdstdout[0] = '\0';

    rv->fstchild.buf[0] = '\0';
    rv->fstchild.childargv[0] = NULL;
    rv->fstchild.prev = NULL;
    rv->fstchild.next = NULL;

    rv->lastchild = &rv->fstchild;

    return rv;
}

static child_t *newchild(void)
{
    child_t *rv;
    if ((rv = malloc(sizeof(child_t))) == NULL) {
        return NULL;
    }
    rv->buf[0] = '\0';
    rv->childargv[0] = NULL;
    rv->prev = NULL;
    rv->next = NULL;

    return rv;
}

static char *readfilename(char *dst)
{
    char buf[STORAGE];
    token_t rc;

    rc = getword(buf);
    if (rc == tok_errnomatch) {
        parseerrno = noquote;
        return NULL;
    }
    if (rc == tok_newline || rc == tok_semi || rc == tok_eof) {
        parseerrno = nofile;
        return NULL;
    }
    if (rc == tok_pipe || rc == tok_lt || rc == tok_gt || rc == tok_amp
        || rc == tok_gtbang) {
        parseerrno = badfile;
        return NULL;
    }
    return strncpy(dst, buf, STORAGE);
}

static void flushline(void)
{
    char buf[STORAGE];
    token_t rc;

    while ((rc = getword(buf)) != tok_newline && rc != tok_amp && rc != tok_eof) {
        ;
    }
}
