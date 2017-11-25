#include "exec.h"

void execcmd(cmd_t *cmd)
{
    child_t *lastchild, *currchild;
    char *infile, *outfile;
    int cstatus, realstdout;
    pid_t cpid;
    int pipefds[2];

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
            exit(10);
        }

        for (currchild = lastchild; currchild->prev != NULL; currchild = currchild->prev) {
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
                exit(8);
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
                    exit(12);
                }
                if (dup2(pipefds[0], STDIN_FILENO) == -1) {
                    perror("dup2");
                    exit(10);
                }
                if (close(pipefds[0]) == -1) {
                    perror("close");
                    exit(12);
                }
                /* Close unnecessary files */
                closecfds();
                /* Fire up the child */
                execvp(currchild->buf, currchild->childargv);
                perror(currchild->buf);
                exit(11);
            } else {
                /* Setup pipe */
                if (close(pipefds[0]) == -1) {
                    perror("close");
                    exit(12);
                }
                if (dup2(pipefds[1], STDOUT_FILENO) == -1) {
                    perror("dup2");
                    exit(10);
                }
                if (close(pipefds[1]) == -1) {
                    perror("close");
                    exit(12);
                }
            }
        }

        /* First child in the pipeline */

        /* Redirect stdin */
        if (dup2(cstdinfd, STDIN_FILENO) == -1) {
            perror("dup2");
            exit(10);
        }
        closecfds();
        if (isbuiltin(currchild->buf)) {
            exit(runbuiltin(currchild));
        }
        execvp(currchild->buf, currchild->childargv);
        perror(currchild->buf);
        exit(11);
    }
    closecfds();
}
