#include "cd.h"

int cd(char *dir)
{
    if (chdir(dir) == -1) {
        return ECHDIR;
    }
    return 0;
}