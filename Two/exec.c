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

void execcmd(cmd_t *cmd)
{
    child_t *lastchild;
    char *infile, *outfile;
    int cstatus, realstdout;
    pid_t cpid;

    infile = cmd->cmdstdin[0] == '\0' ? NULL : cmd->cmdstdin;
    outfile = cmd->cmdstdout[0] == '\0' ? NULL : cmd->cmdstdout;
    if (!mkredirects(infile, outfile, cmd->clobber)) {
        /* Failed to create redirects */
        return;
    }

    /*
     * Why are we setting up the pipeline in reverse order?
     *
     * First, it is when the last executable in a pipeline dies which determines
     * when the pipeline has terminated.
     *
     * We want to spawn the processes in the reverse order of how they are
     * entered in the pipeline. That way when we find that our only child to has
     * died we know that the pipeline is terminated since that only child is the
     * last executable in the pipeline.
     */
    lastchild = cmd->lastchild;

    /*
     * No need to fork if we are running a builtin command which is not in a
     * pipeline
     */
    if (isbuiltin(lastchild->buf)) {
        /*
         * Save our stdout to a different fd. It will be overwritten for the
         * builtin
         */
        if ((realstdout = dup(STDOUT_FILENO)) == -1) {
            perror("dup");
            return;
        }
        if (dup2(cstdoutfd, STDOUT_FILENO) == -1) {
            perror("dup2");
            return;
        }
        runbuiltin(lastchild);
        if (dup2(realstdout, STDOUT_FILENO) == -1) {
            perror("dup2");
            return;
        }
        if (close(realstdout) == -1) {
            perror("close");
            return;
        }
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
        if (dup2(cstdoutfd, STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(9);
        }

        if (lastchild->prev == NULL) {
            /* Simple command; no pipeline */

            /* Redirect stdin */
            if (dup2(cstdinfd, STDIN_FILENO) == -1) {
                perror("dup2");
                exit(9);
            }
            closecfds();
            execvp(lastchild->buf, lastchild->childargv);
            perror(lastchild->buf);
            exit(9);
        }

        execchild(lastchild);
    }
    closecfds();
}

/* XXX: Should be able to work this into execcmd somehow */
static void execchild(child_t *currchild)
{
    pid_t cpid;
    int pipefds[2];

    if (currchild->prev == NULL) {
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

    if (pipe(pipefds) == -1) {
        perror("pipe");
        exit(9);
    }
    cpid = fork();
    if (cpid < 0) {
        perror("fork");
        exit(9);
    }
    if (cpid > 0) {
        /* Setup pipe */
        if (close(pipefds[1]) == -1) {
            perror("close");
            exit(9);
        }
        if (dup2(pipefds[0], STDIN_FILENO) == -1) {
            perror("dup2");
            exit(9);
        }
        if (close(pipefds[0]) == -1) {
            perror("close");
            exit(9);
        }
        /* Close unnecessary files */
        closecfds();
        /* Fire up the child */
        execvp(currchild->buf, currchild->childargv);
        perror(currchild->buf);
        exit(9);
    } else {
        /* Setup pipe */
        if (close(pipefds[0]) == -1) {
            perror("close");
            exit(9);
        }
        if (dup2(pipefds[1], STDOUT_FILENO) == -1) {
            perror("dup2");
            exit(9);
        }
        if (close(pipefds[1]) == -1) {
            perror("close");
            exit(9);
        }
        execchild(currchild->prev);
        /* If we get this far, we are the first executable in the pipeline */
        if (dup2(cstdinfd, STDIN_FILENO) == -1) {
            perror("dup2");
            exit(9);
        }
        closecfds();
        if (isbuiltin(currchild->prev->buf)) {
            exit(runbuiltin(currchild->prev));
        }
        execvp(currchild->prev->buf, currchild->prev->childargv);
        perror(currchild->prev->buf);
        exit(9);
    }
}
