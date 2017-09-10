#include "ls.h"

int ls(FILE *stream, char *dir)
{
    DIR *entry;
    struct dirent *dp;
    int i;

    if ((entry = opendir(dir)) == NULL) {
        return EOPEN;
    }
    i = 0;
    while ((dp = readdir(entry)) != NULL) {
        fprintf(stream, "%s\n", dp->d_name);
        ++i;
    }
    if (closedir(entry) == -1) {
        return ECLOSE;
    }
    return i;
}
