#include "cd.h"

int cd(char *dir) {
    if (dir == NULL) {
        dir = getenv("HOME");
    }
    if (chdir(dir) == -1) {
        return ECHDIR;
    }
    return 0;
}
