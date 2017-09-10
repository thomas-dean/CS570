#ifndef LS_H
#define LS_H

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

#define ECLOSE (-1)
#define EOPEN (-2)

/*
 * Prints the contents of the directory `dir` to `stream`. Returns the number of
 * files in the directory, or < 0 on error. See errno for which error was
 * encountered.
 */
int ls (FILE *stream, char *dir);

#endif /* LS_H */
