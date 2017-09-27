#include "exec.h"

/*
 * Spawn the previous executable in the pipeline
 *
 * Only returns if you pass a currchild without a next child in the list.
 * Spawns child pointed to by currchild.
 *
 * Assumes that the process has already setup its own stdout.
 *
 * This will recursively spawn all children in the pipe
 */
static void execchild(child_t *currchild);
/*
 * Reverse the linked list of children pointed to by head
 */
static void reverse(child_t **head);
static void reverse_helper(child_t *curr, child_t *prev, child_t **head);
static bool isbuiltin(char *exename);
/*
 * Runs the builtin command `child`
 */
static int runbuiltin(child_t *child);

void execcmd(cmd_t *cmd)
{
    child_t *lastchild;
    char *infile, *outfile;
    int cstatus, realstdout;
    pid_t cpid;

    infile = cmd->cmdstdin[0] == '\0' ? NULL : cmd->cmdstdin;
    outfile = cmd->cmdstdout[0] == '\0' ? NULL : cmd->cmdstdout;
    if (!mkredirects(infile, outfile, cmd->clobber)) {
        return;
    }

    /*
     * Why are we reversing the order of the pipeline?
     *
     * First, it is when the last executable in a pipeline dies which determines
     * when the pipeline has terminated.
     *
     * We want to spawn the processes in the reverse order of how they are
     * entered in the pipeline. That way when we find that our only child to has
     * died we know that the pipeline is terminated since that only child is the
     * last executable in the pipeline.
     */
    lastchild = &cmd->fstchild;
    reverse(&lastchild);

    /*
     * No need to fork if we are running a builtin command which is not in a
     * pipeline
     */
    if (isbuiltin(lastchild->buf)) {
        /*
         * Save our stdout to a different fd. It will be overwritten for the
         * builtin
         */
        realstdout = dup(STDOUT_FILENO);
        dup2(cstdoutfd, STDOUT_FILENO);
        runbuiltin(lastchild);
        dup2(realstdout, STDOUT_FILENO);
        return;
    }

    /*
     * Now we fork off our first child (the last executable in the pipeline.
     * This child will setup its redirection (if necessary) and fork the
     * previous executable in the pipeline (if it exists).
     */
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        return;
    }
    if (cpid > 0) {
        /* Parent */
        if (!cmd->runinbg) {
            while (wait(&cstatus) != cpid) {
                ;
            }
        } else {
            printf("%s [%ld]\n", cmd->fstchild.buf, (long)cpid);
        }
    } else {
        /* Child */

        /*
         * The first child should setup the stdout redirection for the command.
         */
        dup2(cstdoutfd, STDOUT_FILENO);

        if (lastchild->next == NULL) {
            /* Simple command; no pipeline */

            /* Redirect stdin */
            dup2(cstdinfd, STDIN_FILENO);
            closecfds();
            execvp(lastchild->buf, lastchild->childargv);
            perror(lastchild->buf);
            exit(9);
        }

        if (lastchild->next != NULL) {
            execchild(lastchild);
            /* I'm not the first one in the pipe, so we cannot get here */
        }
    }
    closecfds();
}

/* XXX: Should be able to work this into execcmd somehow */
static void execchild(child_t *currchild)
{
    pid_t cpid;
    int pipefds[2];

    if (currchild->next == NULL) {
        return;
    }

    if (isbuiltin(currchild->buf)) {
        /*
         * NOTE: If we get to this point we are going to stop executing the
         * pipeline and just run the builtin as if it was the first executable
         * in the pipline. This means any executables before this builtin will
         * not be run
         */
        /* We are a forked child, so its OK for us to die */
        exit(runbuiltin(currchild));
    }

    pipe(pipefds);
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        exit(9);
    }
    if (cpid > 0) {
        /* Setup pipe */
        close(pipefds[1]);
        dup2(pipefds[0], STDIN_FILENO);
        close(pipefds[0]);
        /* Close unnecessary files */
        closecfds();
        /* Fire up the child */
        execvp(currchild->buf, currchild->childargv);
        perror(currchild->buf);
        exit(9);
    } else {
        /* Setup pipe */
        close(pipefds[0]);
        dup2(pipefds[1], STDOUT_FILENO);
        close(pipefds[1]);
        execchild(currchild->next);
        /* If we get this far, we are the first executable in the pipeline */
        dup2(cstdinfd, STDIN_FILENO);
        closecfds();
        if (isbuiltin(currchild->next->buf)) {
            exit(runbuiltin(currchild->next));
        }
        execvp(currchild->next->buf, currchild->next->childargv);
        perror(currchild->next->buf);
        exit(9);
    }
}

static void reverse(child_t **head)
{
    if (head == NULL) {
        return;
    }
    reverse_helper(*head, NULL, head);
}

static void reverse_helper(child_t *curr, child_t *prev, child_t **head)
{
    if (curr->next == NULL)
    {
        *head = curr;
        curr->next = prev;
        return;
    }
    child_t *next = curr->next;
    curr->next = prev;
    reverse_helper(next, curr, head);
}

static bool isbuiltin(char *exename)
{
    static char *builtins[] = {
        "cd",
        "ls-F",
        NULL
    };
    char **bp;

    for (bp = builtins; *bp; ++bp) {
        if (strcmp(*bp, exename) == 0) {
            return true;
        }
    }
    return false;
}

static int runbuiltin(child_t *child)
{
    int argc;

    for (argc = 0; child->childargv[argc]; ++argc) {
        ;
    }
    if (strcmp(child->buf, "cd") == 0) {
        if (argc > 2) {
            fprintf(stderr, "Too many arguments to cd\n");
        }
        return cd(child->childargv[1]);
    } else if (strcmp(child->buf, "ls-F") == 0) {
        if (argc > 2) {
            fprintf(stderr, "Too many arguments to ls-F\n");
        }
        /* XXX: Change this to wherever we are supposed to write output */
        return ls(STDOUT_FILENO, child->childargv[1]);
    } else {
        fprintf(stderr, "Internal error: failed to find builtin for %s\n", child->buf);
        return -1;
    }
}
