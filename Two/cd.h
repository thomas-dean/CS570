#ifndef CD_H
#define CD_H

#include <unistd.h>

#define ECHDIR (-1)

/*
 * Change the processes current directory. Returns < 0 on error. See errno for
 * which error was encountered.
 */
int cd(const char *dir);

#endif /* CD_H */
