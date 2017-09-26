#include "redir.h"

int cstdinfd, cstdoutfd;

bool mkredirects(char *infile, char *outfile, bool clobber)
{
    int oflags;
    mode_t umask = S_IWUSR | S_IRUSR | S_IWGRP | S_IRGRP;
    cstdoutfd = STDOUT_FILENO;
    cstdinfd = STDIN_FILENO;

    /* check if we should overwrite stdin */
    if (infile != NULL) {
        oflags = O_RDONLY;
        if ((cstdinfd = open(infile, oflags)) < 0) {
            perror(infile);
            return false;
        }
    }

    /* check if we should overwrite stdout */
    if (outfile != NULL) {
        oflags = O_WRONLY | O_CREAT | O_TRUNC;
        if (!clobber) {
            /* fail if the file
             * already exists */
            oflags |= O_EXCL;
        }
        if ((cstdoutfd = open(outfile, oflags, umask)) < 0) {
            /* Could have opened a file for stdin, close it */
            if (cstdinfd != STDIN_FILENO) {
                close(cstdinfd);  /* NOTE: This could fail, but we don't care */
            }
            perror(outfile);
            return false;
        }
    }
    return true;
}

bool closecfds(void)
{
    bool rv = false;

    if (cstdinfd != STDIN_FILENO) {
        if (close(cstdinfd) == -1) {
            rv = true;
        }
    }
    if (cstdoutfd != STDOUT_FILENO) {
        if (close(cstdoutfd) == -1) {
            rv = true;
        }
    }
    return rv;
}

