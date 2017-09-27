#ifndef BUILTINS_H
#define BUILTINS_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define ECLOSE (-1)
#define EOPEN (-2)
#define ECHDIR (-1)

/*
 * Prints the contents of the directory `dir` to file descriptor `outfd`.
 * Returns the number of files in the directory, or < 0 on error.
 */
int ls (int outfd, const char *dir);

/*
 * Change the processes current directory. Returns < 0 on error. See errno for
 * which error was encountered.
 */
int cd(char *dir);

#endif /* BUILTINS_H */
