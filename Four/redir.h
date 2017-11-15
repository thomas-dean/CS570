#ifndef REDIR_H
#define REDIR_H

#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

/*
 * The stdin and stdout file descriptors which should be used for the command
 * which will be created
 */
extern int cstdinfd, cstdoutfd;

/*
 * Opens files for infile and outfile (if non-null) as cstdinfd and cstdoutfd
 * respectively. If clobber is false, fail if outfile already exists.
 * Returns true on success, false on failure
 */
bool mkredirects(char *infile, char *outfile, bool clobber);

/*
 * Closes file descriptors which are not the usual std{in,out,err}
 * Returns true on success, false on failure
 */
bool closecfds(void);

#endif /* REDIR_H */
