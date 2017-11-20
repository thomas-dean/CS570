#ifndef BUILTINS_H
#define BUILTINS_H

#include "parse.h"

#include <dirent.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ECLOSE (-1)
#define EOPEN (-2)
#define ECHDIR (-1)

/*
 * Returns true if the executable name `exename` is a builtin command.
 * Otherwise, it returns false
 */
bool isbuiltin(char *exename);

/*
 * Runs the builtin command `child`. Returns 0 on success; < 0 on failure.
 */
int runbuiltin(child_t *child);

#endif /* BUILTINS_H */
